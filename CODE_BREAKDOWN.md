# 📖 AK URL Shortener — Complete Code Breakdown

A beginner-friendly, in-depth explanation of every part of this project — from scratch.

---

## 🗺️ Big Picture: How Everything Connects

```
┌─────────────────────────────────────────────────────────┐
│                    YOUR BROWSER                         │
│  index.html / shorten.html / analytics.html / history  │
│  (HTML + CSS + JavaScript)                              │
└────────────────────┬────────────────────────────────────┘
                     │  HTTP requests (fetch API)
                     ▼
┌─────────────────────────────────────────────────────────┐
│              Node.js Server  (server/server.js)         │
│  • Receives API requests                                │
│  • Runs URL shortening logic                            │
│  • Serves the frontend files                            │
└────────────────────┬────────────────────────────────────┘
                     │  (in-memory, same process)
                     ▼
┌─────────────────────────────────────────────────────────┐
│              C++ Engine  (url-shortener-cpp/)           │
│  • Same algorithms, implemented in C++ for learning     │
│  • Compiled separately, runs as a CLI demo              │
└─────────────────────────────────────────────────────────┘
```

**In simple terms:**
- You open a webpage → it sends a request to the Node.js server → the server shortens the URL and sends back a short code → you share that short code → anyone who visits it gets redirected.

---

## 🧩 Core Algorithm: How a URL Gets Shortened

### Step 1 — Generate a Unique ID

Every URL gets a unique number. We use a simple counter:

```
URL #1 → ID: 1
URL #2 → ID: 2
URL #3 → ID: 3
...
```

**C++ file:** `core/Idgenerator.cpp`
```cpp
std::atomic<long long> counter;  // atomic = thread-safe
long long getNextId() {
    return counter.fetch_add(1); // returns current, then adds 1
}
```

**Why `atomic`?** If two requests come in at the same time (multi-threading), a regular integer could give the same ID to both. `atomic` prevents that.

---

### Step 2 — Base62 Encode the ID

We convert the number into a short string using 62 characters: `0-9`, `a-z`, `A-Z`.

```
ID: 1      → "1"
ID: 62     → "10"   (like binary, but base 62)
ID: 12345  → "dnh"
```

**Why Base62?**
- Only URL-safe characters (no `?`, `&`, `#`, etc.)
- Very compact: 7 characters can represent **3.5 trillion** URLs
- No ambiguous characters like `0` vs `O`

**C++ file:** `core/Base62Encoder.cpp`
```cpp
string encode(long long number) {
    string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string result = "";
    while (number > 0) {
        result = chars[number % 62] + result;  // get remainder
        number = number / 62;                  // divide
    }
    return result;
}
```

**Same logic in Node.js:** `server/server.js` → `base62Encode()`

---

### Step 3 — Store the Mapping

We store: `"dnh"` → `"https://google.com"`

**C++ file:** `core/urlRepository.cpp`
```cpp
unordered_map<string, UrlEntry> store;
// unordered_map = hash table = O(1) lookup
```

**Node.js:** `server.js` → `const urlStore = new Map()`

---

### Step 4 — Redirect

When someone visits `localhost:3000/dnh`:
1. Look up `"dnh"` in the store
2. Return `302 redirect` to `https://google.com`

```
Browser → GET /dnh → Server → 302 → https://google.com
```

---

## ⚡ LRU Cache — Why and How

### The Problem
If 1 million people visit the same short link, we'd hit the database 1 million times. That's slow and expensive.

### The Solution: Cache
Keep the **most recently used** URLs in fast memory. Only go to the database on a cache miss.

```
Request for "dnh"
    │
    ▼
┌─────────────┐   HIT   ┌──────────────────┐
│  LRU Cache  │ ──────► │ Return instantly  │ ← < 1ms
│  (memory)   │         └──────────────────┘
└─────────────┘
    │ MISS
    ▼
┌─────────────┐         ┌──────────────────┐
│  URL Store  │ ──────► │ Return + warm     │ ← ~1ms
│  (Map/DB)   │         │ the cache         │
└─────────────┘         └──────────────────┘
```

### How LRU Works

**LRU = Least Recently Used** — when the cache is full, evict the URL that hasn't been accessed the longest.

```
Cache size = 3. Access order: A, B, C, D

After A:  [A]
After B:  [B, A]
After C:  [C, B, A]
After D:  [D, C, B]  ← A evicted (least recently used)
Access B: [B, D, C]  ← B moved to front
```

**Data structure used:** `doubly linked list` + `hash map`
- Hash map: O(1) lookup by key
- Linked list: O(1) move-to-front and evict-from-back

**C++ file:** `core/LRUCache.cpp`
```cpp
list<string> order;           // doubly linked list (front = recent)
unordered_map<string,
  pair<string, list<string>::iterator>> cache;
// map: key → (value, pointer to its position in list)
```

---

## 🛡️ Rate Limiter — Token Bucket Algorithm

### The Problem
Without limits, someone could send millions of requests per second and crash the server (DDoS attack).

### The Solution: Token Bucket

Imagine a bucket that holds tokens:
- Bucket starts full (e.g., 10 tokens)
- Each request costs 1 token
- Tokens refill at a fixed rate (e.g., 3/second)
- If bucket is empty → request is **blocked**

```
Time 0s:  [●●●●●●●●●●]  10 tokens
Request:  [●●●●●●●●●]   9 tokens (1 used)
Request:  [●●●●●●●●]    8 tokens
...
Time 1s:  [●●●●●●●●●●●] refill +3 → capped at 10
```

**C++ file:** `core/RateLimiter.cpp`
```cpp
void refill(Bucket& b) {
    double elapsed = time_since_last_refill;
    b.tokens = min(maxTokens, b.tokens + elapsed * refillRate);
}

bool allowRequest(string ip) {
    refill(bucket[ip]);
    if (bucket[ip].tokens >= 1) {
        bucket[ip].tokens -= 1;
        return true;   // allowed
    }
    return false;      // blocked
}
```

**Each IP gets its own bucket** — so one abuser doesn't affect others.

---

## ⏱️ URL Expiry (TTL)

TTL = **Time To Live** — how long a URL stays valid.

```cpp
struct UrlEntry {
    string longUrl;
    time_point expiresAt;  // when it dies
    bool hasExpiry;
};
```

When `find()` is called:
```cpp
if (entry.hasExpiry && now > entry.expiresAt) {
    store.erase(shortCode);  // auto-delete
    return "";               // expired
}
```

**Use cases:**
- Promo links that expire after a sale
- Event links that expire after the event
- Temporary file sharing links

---

## 🔄 Consistent Hashing — Distributed Storage

### The Problem
What if you have 3 database servers and need to decide which server stores which URL?

**Naive approach:** `server = id % 3`
- Server 0 → IDs: 0, 3, 6, 9...
- Server 1 → IDs: 1, 4, 7, 10...
- Problem: if you add a 4th server, **all** mappings change!

### The Solution: Consistent Hashing

Place servers on a **ring** (0 to 2³²). Each URL hashes to a point on the ring, and goes to the **nearest server clockwise**.

```
        Node 1
          │
   ───────●───────
  /                \
●                   ●
Node 3           Node 2
  \                /
   ───────────────
```

When you add/remove a node, only the URLs near that node move — not everything.

**C++ file:** `core/consistenthashing.cpp`
```cpp
// Virtual nodes: each real node gets 3 positions on the ring
// This ensures even distribution
ring[hash("NODE_1_0")] = 1;
ring[hash("NODE_1_1")] = 1;
ring[hash("NODE_1_2")] = 1;
```

---

## 📊 Analytics Tracker

Counts how many times each short URL was clicked.

```cpp
unordered_map<string, long long> hitCounts;

void recordHit(string shortCode) {
    hitCounts[shortCode]++;
}
```

**In Node.js:** `entry.clicks++` every time `redirect()` is called.

---

## 🌐 Node.js Server — How the API Works

**File:** `server/server.js`

### POST /api/shorten
```
Browser sends:  { longUrl: "https://google.com", ttlSeconds: 3600 }
Server does:    1. Rate limit check
                2. Validate URL format
                3. Generate ID → Base62 encode
                4. Save to urlStore
Server returns: { shortCode: "1", shortUrl: "http://localhost:3000/1" }
```

### GET /:code (redirect)
```
Browser visits: http://localhost:3000/1
Server does:    1. Check LRU cache
                2. If miss → check urlStore
                3. If expired → return 404
                4. Record click (analytics)
                5. Return 302 redirect
Browser goes:   https://google.com  ← automatically
```

### GET /api/analytics
Returns summary stats: total URLs, total clicks, top URL, active count.

### GET /api/urls
Returns full list of all URLs with their metadata.

### DELETE /api/urls/:code
Deletes a specific URL from the store and cache.

---

## 🖥️ Frontend — How Each Page Works

### index.html (Landing Page)
- Shows hero section with quick-shorten bar
- Calls `POST /api/shorten` when you click "Shorten"
- Calls `GET /api/analytics` to show live stats

### shorten.html (Full Form)
- Full options: custom alias, TTL selector
- Calls `POST /api/shorten` with all options
- Shows result with copy button and QR code modal
- QR code drawn on `<canvas>` using math (no external library)

### analytics.html (Dashboard)
- Calls `GET /api/analytics` for stat cards
- Calls `GET /api/urls` for the bar chart and table
- Auto-refreshes every 5 seconds

### history.html (URL List)
- Calls `GET /api/urls` for all links
- Search, filter (active/expired), sort (newest/clicks)
- TTL progress bar shows remaining lifetime
- Calls `DELETE /api/urls/:code` to delete
- QR code modal for any URL

---

## 🧵 Thread Safety — Why It Matters

When multiple requests come in simultaneously (concurrent users), they might try to read/write the same data at the same time. This causes **race conditions** — corrupted data.

**Solution:** `std::mutex` — a lock that only one thread can hold at a time.

```cpp
void put(string key, string value) {
    lock_guard<mutex> lock(mtx);  // lock acquired
    // ... modify cache ...
}  // lock automatically released here
```

**Files with mutex:** `LRUCache.cpp`, `urlRepository.cpp`, `RateLimiter.cpp`, `AnalyticsTracker.cpp`

---

## 🗂️ Complete File Map

```
url_shortner/
│
├── url-shortener-cpp/          ← C++ CLI demo (Phase 1-4)
│   ├── core/
│   │   ├── Base62Encoder.*     ← Number → short string
│   │   ├── Idgenerator.*       ← Unique ID counter
│   │   ├── LRUCache.*          ← Fast in-memory cache
│   │   ├── urlrespository.h    ← Storage with TTL
│   │   ├── urlRepository.cpp   ← Storage implementation
│   │   ├── RateLimiter.*       ← Token bucket per IP
│   │   ├── consistenthashing.* ← Distributed node ring
│   │   ├── AnalyticsTracker.*  ← Click counting
│   │   ├── QRCodeStub.h        ← ASCII QR placeholder
│   │   └── urlshortener*.*     ← Main orchestrator
│   └── main.cpp                ← Demo runner
│
├── server/                     ← Node.js HTTP backend
│   ├── server.js               ← All API logic + server
│   └── package.json            ← Dependencies
│
├── frontend/                   ← Web UI
│   ├── index.html              ← Landing page
│   ├── shorten.html            ← URL shortener form
│   ├── analytics.html          ← Dashboard
│   ├── history.html            ← URL history + QR
│   └── css/style.css           ← Shared design system
│
├── README.md                   ← Backend docs (per phase)
└── FRONTEND.md                 ← Frontend docs (per phase)
```

---

## 🚀 How to Run

```bash
# Start the server (from project root)
cd server
node server.js

# Open in browser
http://localhost:3000
```

**That's it.** The server serves both the API and the frontend files.

---

## 🎓 Learning Path (Read in This Order)

1. **`Base62Encoder.cpp`** — simplest file, understand the encoding math
2. **`Idgenerator.cpp`** — atomic counter, understand thread safety basics
3. **`urlRepository.cpp`** — simple hash map storage with TTL
4. **`LRUCache.cpp`** — most complex data structure, doubly linked list + map
5. **`RateLimiter.cpp`** — token bucket algorithm
6. **`urlshortservice.cpp`** — how all pieces connect together
7. **`server.js`** — same concepts in JavaScript, plus HTTP layer
8. **`shorten.html`** — how the frontend calls the API
