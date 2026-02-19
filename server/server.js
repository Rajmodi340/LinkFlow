/**
 * AK URL Shortener — Node.js HTTP Backend
 * Implements: Base62 encoding, LRU cache, URL expiry (TTL),
 *             rate limiting (token bucket), custom alias, analytics
 */

const express = require('express');
const cors    = require('cors');
const path    = require('path');

const app  = express();
const PORT = 3000;

app.use(cors());
app.use(express.json());

// Serve frontend static files
app.use(express.static(path.join(__dirname, '..', 'frontend')));

// ─────────────────────────────────────────────
// Base62 Encoder
// ─────────────────────────────────────────────
const BASE62 = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
function base62Encode(n) {
  if (n === 0) return '0';
  let result = '';
  while (n > 0) {
    result = BASE62[n % 62] + result;
    n = Math.floor(n / 62);
  }
  return result;
}

// ─────────────────────────────────────────────
// ID Generator (auto-incrementing)
// ─────────────────────────────────────────────
let counter = 1;
function getNextId() { return counter++; }

// ─────────────────────────────────────────────
// LRU Cache
// ─────────────────────────────────────────────
class LRUCache {
  constructor(capacity = 100) {
    this.capacity = capacity;
    this.map = new Map(); // key → value (Map preserves insertion order)
  }
  get(key) {
    if (!this.map.has(key)) return null;
    const val = this.map.get(key);
    // Move to end (most recently used)
    this.map.delete(key);
    this.map.set(key, val);
    return val;
  }
  put(key, value) {
    if (this.map.has(key)) this.map.delete(key);
    else if (this.map.size >= this.capacity) {
      // Evict LRU (first entry)
      this.map.delete(this.map.keys().next().value);
    }
    this.map.set(key, value);
  }
  remove(key) { this.map.delete(key); }
}

// ─────────────────────────────────────────────
// URL Repository (in-memory with TTL)
// ─────────────────────────────────────────────
const urlStore = new Map(); // shortCode → { longUrl, createdAt, expiresAt, clicks }

function saveUrl(shortCode, longUrl, ttlSeconds = 0) {
  urlStore.set(shortCode, {
    longUrl,
    createdAt: Date.now(),
    expiresAt: ttlSeconds > 0 ? Date.now() + ttlSeconds * 1000 : 0,
    ttlSeconds,
    clicks: 0
  });
}

function findUrl(shortCode) {
  const entry = urlStore.get(shortCode);
  if (!entry) return null;
  if (entry.expiresAt && Date.now() > entry.expiresAt) {
    urlStore.delete(shortCode);
    cache.remove(shortCode);
    return null; // expired
  }
  return entry;
}

function existsCode(shortCode) {
  return urlStore.has(shortCode);
}

// ─────────────────────────────────────────────
// LRU Cache instance
// ─────────────────────────────────────────────
const cache = new LRUCache(100);

// ─────────────────────────────────────────────
// Rate Limiter (Token Bucket per IP)
// ─────────────────────────────────────────────
const buckets = new Map(); // ip → { tokens, lastRefill }
const MAX_TOKENS   = 10;  // burst
const REFILL_RATE  = 3;   // tokens per second

function allowRequest(ip) {
  const now = Date.now() / 1000;
  if (!buckets.has(ip)) {
    buckets.set(ip, { tokens: MAX_TOKENS - 1, lastRefill: now });
    return true;
  }
  const b = buckets.get(ip);
  const elapsed = now - b.lastRefill;
  b.tokens = Math.min(MAX_TOKENS, b.tokens + elapsed * REFILL_RATE);
  b.lastRefill = now;
  if (b.tokens >= 1) { b.tokens -= 1; return true; }
  return false;
}

// ─────────────────────────────────────────────
// Analytics
// ─────────────────────────────────────────────
function recordHit(shortCode) {
  const entry = urlStore.get(shortCode);
  if (entry) entry.clicks++;
}

// ─────────────────────────────────────────────
// API Routes
// ─────────────────────────────────────────────

// POST /api/shorten
app.post('/api/shorten', (req, res) => {
  const ip = req.headers['x-forwarded-for'] || req.socket.remoteAddress || 'unknown';

  if (!allowRequest(ip)) {
    return res.status(429).json({ error: 'Rate limit exceeded. Try again shortly.' });
  }

  const { longUrl, customAlias, ttlSeconds = 0 } = req.body;

  if (!longUrl) return res.status(400).json({ error: 'longUrl is required' });

  // Validate URL
  try { new URL(longUrl); } catch {
    return res.status(400).json({ error: 'Invalid URL format' });
  }

  let shortCode;
  if (customAlias) {
    if (!/^[a-zA-Z0-9_-]{1,30}$/.test(customAlias)) {
      return res.status(400).json({ error: 'Alias must be alphanumeric (max 30 chars)' });
    }
    if (existsCode(customAlias)) {
      return res.status(409).json({ error: `Alias "${customAlias}" is already taken` });
    }
    shortCode = customAlias;
  } else {
    shortCode = base62Encode(getNextId());
  }

  saveUrl(shortCode, longUrl, parseInt(ttlSeconds));

  res.json({
    shortCode,
    shortUrl: `http://localhost:${PORT}/${shortCode}`,
    longUrl,
    ttlSeconds: parseInt(ttlSeconds),
    expiresAt: ttlSeconds > 0 ? new Date(Date.now() + ttlSeconds * 1000).toISOString() : null,
    createdAt: new Date().toISOString()
  });
});

// GET /:code — redirect
app.get('/:code', (req, res) => {
  const { code } = req.params;

  // Skip static file routes
  if (code.includes('.') || ['api'].includes(code)) return res.status(404).send('Not found');

  // Check cache first
  let longUrl = cache.get(code);
  if (!longUrl) {
    const entry = findUrl(code);
    if (!entry) {
      return res.status(404).send(`
        <html><body style="font-family:sans-serif;text-align:center;padding:4rem;background:#0a0a14;color:#f1f5f9;">
          <h2>❌ Link not found or expired</h2>
          <p style="color:#94a3b8;">This short URL does not exist or has expired.</p>
          <a href="/" style="color:#7c3aed;">← Go Home</a>
        </body></html>
      `);
    }
    longUrl = entry.longUrl;
    cache.put(code, longUrl);
  }

  recordHit(code);
  res.redirect(302, longUrl);
});

// GET /api/urls — list all URLs
app.get('/api/urls', (req, res) => {
  const now = Date.now();
  const urls = [];
  for (const [code, entry] of urlStore.entries()) {
    const expired = entry.expiresAt && now > entry.expiresAt;
    urls.push({
      shortCode: code,
      shortUrl: `http://localhost:${PORT}/${code}`,
      longUrl: entry.longUrl,
      clicks: entry.clicks,
      createdAt: new Date(entry.createdAt).toISOString(),
      expiresAt: entry.expiresAt ? new Date(entry.expiresAt).toISOString() : null,
      ttlSeconds: entry.ttlSeconds,
      expired
    });
  }
  // Sort newest first
  urls.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
  res.json(urls);
});

// GET /api/analytics — summary stats
app.get('/api/analytics', (req, res) => {
  const now = Date.now();
  let totalClicks = 0, activeCount = 0, topUrl = null, topClicks = 0;

  for (const [code, entry] of urlStore.entries()) {
    totalClicks += entry.clicks;
    const expired = entry.expiresAt && now > entry.expiresAt;
    if (!expired) activeCount++;
    if (entry.clicks > topClicks) { topClicks = entry.clicks; topUrl = code; }
  }

  res.json({
    totalUrls: urlStore.size,
    totalClicks,
    activeUrls: activeCount,
    topUrl,
    topClicks
  });
});

// DELETE /api/urls/:code — delete a URL
app.delete('/api/urls/:code', (req, res) => {
  const { code } = req.params;
  if (!urlStore.has(code)) return res.status(404).json({ error: 'Not found' });
  urlStore.delete(code);
  cache.remove(code);
  res.json({ success: true });
});

// DELETE /api/urls — clear all
app.delete('/api/urls', (req, res) => {
  urlStore.clear();
  res.json({ success: true });
});

// ─────────────────────────────────────────────
// Start server
// ─────────────────────────────────────────────
app.listen(PORT, () => {
  console.log(`\n🚀 AK URL Shortener running at http://localhost:${PORT}`);
  console.log(`   Frontend  → http://localhost:${PORT}/index.html`);
  console.log(`   Shorten   → POST http://localhost:${PORT}/api/shorten`);
  console.log(`   Redirect  → GET  http://localhost:${PORT}/:code`);
  console.log(`   Analytics → GET  http://localhost:${PORT}/api/analytics\n`);
});
