# 🚀 Advanced URL Shortener (C++)

A high-performance URL shortening service built in C++, implemented in **4 progressive phases** — from a core in-memory engine to a full-featured service with rate limiting, analytics, and distributed hashing.

---

## 📋 Table of Contents
- [Phase 1 — Core Engine](#phase-1--core-engine)
- [Phase 2 — Rate Limiting, URL Expiry & Thread Safety](#phase-2--rate-limiting-url-expiry--thread-safety)
- [Phase 3 — Consistent Hashing & Distribution](#phase-3--consistent-hashing--distribution)
- [Phase 4 — Advanced Features](#phase-4--advanced-features)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)

---

## Phase 1 — Core Engine

> **Status: ✅ Complete**

The foundation of the service. All components are in `core/`.

### Components

| File | Responsibility |
|------|---------------|
| `Idgenerator.h/.cpp` | Atomic counter-based unique ID generation (thread-safe) |
| `Base62Encoder.h/.cpp` | Converts numeric IDs → short alphanumeric codes (`[a-zA-Z0-9]`) |
| `LRUCache.h/.cpp` | O(1) in-memory cache with LRU eviction |
| `urlrespository.h` / `urlRepository.cpp` | Storage layer mapping short codes → long URLs |
| `urlshortenerservice.h` / `urlshortservice.cpp` | Main orchestrator |

### How It Works

```
1. shortenUrl("https://google.com")
   └─> ID: 1  →  Base62: "1"  →  store("1", "https://google.com")  →  return "1"

2. redirect("1")
   └─> Check LRU cache → miss → fetch from repository → warm cache → return URL
```

### Why Base62?
- URL-safe characters only (`[a-zA-Z0-9]`)
- 62^7 = **3.5 trillion** possible short codes
- No special characters needing URL encoding

---

## Phase 2 — Rate Limiting, URL Expiry & Thread Safety

> **Status: ✅ Complete**

### Rate Limiting (`RateLimiter.h/.cpp`)

**Token Bucket Algorithm** — per IP address:
- Each IP gets a bucket of `maxTokens` (default: 5 burst)
- Tokens refill at `refillRate` per second (default: 2/sec)
- Request is blocked if bucket is empty

```cpp
RateLimiter limiter(5.0, 2.0);  // 5 burst, 2 tokens/sec refill
limiter.allowRequest("192.168.1.1");  // true or false
```

### URL Expiry / TTL (`urlrespository.h`)

Each URL entry now stores an optional expiry timestamp:

```cpp
// Expires in 60 seconds
service.shortenUrl("https://promo.com", 60 /*ttlSeconds*/);

// No expiry (permanent)
service.shortenUrl("https://google.com");
```

When `redirect()` is called on an expired URL, it returns `""` and auto-deletes the entry.

### Thread Safety
- `LRUCache` — `std::mutex` on all `get()` / `put()` / `remove()` calls
- `UrlRepository` — `std::mutex` on all `save()` / `find()` calls
- `RateLimiter` — `std::mutex` on bucket access
- `AnalyticsTracker` — `std::mutex` on hit recording

---

## Phase 3 — Consistent Hashing & Distribution

> **Status: ✅ Complete**

### Consistent Hashing (`consistenthashing.h/.cpp`)

Distributes short codes across multiple storage nodes with minimal reshuffling when nodes are added/removed.

```cpp
service.addNode(1);
service.addNode(2);
service.addNode(3);

service.printNodeAssignment("abc");  // → Node 2
service.printNodeAssignment("xyz");  // → Node 1
```

**Virtual nodes** (default: 3 per real node) ensure even distribution.

### Scalability Architecture

```
User Request
     │
     ▼
┌──────────────┐
│  Geo DNS     │  ← Routes to nearest region
└──────────────┘
     │
     ├──────────┬──────────┬──────────┐
     ▼          ▼          ▼          ▼
┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐
│ Node 1  │ │ Node 2  │ │ Node 3  │ │ Node N  │
└─────────┘ └─────────┘ └─────────┘ └─────────┘
```

---

## Phase 4 — Advanced Features

> **Status: ✅ Complete**

### Custom Alias Support

```cpp
// Use a human-readable alias instead of auto-generated code
service.shortenUrl("https://linkedin.com/in/akshay", 0, "", "akshay");
service.redirect("akshay");  // → https://linkedin.com/in/akshay
```

Duplicate aliases are rejected with a warning.

### Analytics Tracking (`AnalyticsTracker.h/.cpp`)

Tracks click counts per short code. Prints a sorted report:

```
📊 Analytics Report (Top 5 URLs):
  ┌─────────────┬───────────┐
  │  Short Code │   Clicks  │
  ├─────────────┼───────────┤
  │ 1           │        25 │
  │ akshay      │         5 │
  │ 2           │         8 │
  └─────────────┴───────────┘
```

### QR Code Stub (`QRCodeStub.h`)

ASCII art placeholder for QR code generation. In production, integrate with [libqrencode](https://fukuchi.org/works/qrencode/).

```cpp
QRCodeStub::printQR("akshay");
// Prints ASCII QR art + full URL
```

---

## Getting Started

### Prerequisites
- C++ compiler with **C++17** support (GCC 7+, Clang 5+, MSVC 2017+)

### Compile

```bash
cd url-shortener-cpp
g++ -std=c++17 main.cpp core/*.cpp -o app.exe
```

### Run

```bash
./app.exe
```

The demo runs all 4 phases sequentially, showing:
1. Basic shorten + redirect
2. LRU cache hits
3. Rate limiting (blocked requests)
4. URL expiry (TTL)
5. Consistent hash node assignments
6. Custom aliases
7. QR code ASCII art
8. Analytics dashboard

---

## Project Structure

```
url-shortener-cpp/
├── core/
│   ├── Base62Encoder.h/.cpp        # Phase 1 — Base62 encoding
│   ├── Idgenerator.h/.cpp          # Phase 1 — Unique ID generation
│   ├── LRUCache.h/.cpp             # Phase 1+2 — LRU cache (thread-safe)
│   ├── urlrespository.h            # Phase 1+2 — Storage layer (with TTL)
│   ├── urlRepository.cpp           # Phase 1+2 — Storage implementation
│   ├── RateLimiter.h/.cpp          # Phase 2 — Token bucket rate limiter
│   ├── consistenthashing.h/.cpp    # Phase 3 — Consistent hash ring
│   ├── AnalyticsTracker.h/.cpp     # Phase 4 — Click analytics
│   ├── QRCodeStub.h                # Phase 4 — QR code ASCII stub
│   ├── urlshortenerservice.h       # All phases — Main orchestrator header
│   └── urlshortservice.cpp         # All phases — Main orchestrator impl
├── main.cpp                        # Full demo (all 4 phases)
└── app.exe                         # Compiled binary
```

---

## Performance

| Operation | Latency | Notes |
|-----------|---------|-------|
| Cache Hit | < 1ms | LRU in-memory |
| Cache Miss + Repo | ~1ms | In-memory repo |
| URL Creation | < 1ms | Atomic counter + Base62 |
| Rate Check | < 0.1ms | Token bucket |

---

**Built with ❤️ and C++17**