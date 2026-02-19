# 🌐 URL Shortener — Frontend

A modern, responsive frontend for the Advanced URL Shortener, built in **4 progressive phases** using HTML, CSS, and JavaScript.

---

## 📋 Phase Overview

| Phase | Feature | Status |
|-------|---------|--------|
| Phase 1 | Landing Page (Hero + Features) | ✅ Complete |
| Phase 2 | Shorten URL Form + Result Display | ✅ Complete |
| Phase 3 | Analytics Dashboard UI | ✅ Complete |
| Phase 4 | URL History + QR Code Display | ✅ Complete |

---

## Phase 1 — Landing Page

**File:** `frontend/index.html` + `frontend/css/style.css`

### What's included:
- **Hero section** — headline, subheadline, CTA button ("Shorten a URL")
- **Features section** — 6 feature cards (Fast, Secure, Analytics, Custom Alias, QR Code, Expiry)
- **How It Works** — 3-step visual flow
- **Footer** — links and branding

### Design:
- Dark glassmorphism theme
- Gradient purple/blue palette
- Smooth scroll animations
- Fully responsive (mobile + desktop)

---

## Phase 2 — URL Shortener Form

**File:** `frontend/shorten.html` + `frontend/js/shorten.js`

### What's included:
- **Input form** — long URL field + optional custom alias + TTL selector
- **Shorten button** — with loading spinner
- **Result card** — shows generated short URL with:
  - One-click copy button
  - Share button
  - Link to QR code
- **Validation** — URL format check before submit
- **Error handling** — rate limit / alias taken messages

### Simulated API:
Uses `localStorage` to simulate the C++ backend (Base62 encoding in JS).

---

## Phase 3 — Analytics Dashboard

**File:** `frontend/analytics.html` + `frontend/js/analytics.js`

### What's included:
- **Stats cards** — Total URLs, Total Clicks, Top URL, Cache Hit Rate
- **Top URLs table** — sortable by clicks, with short code + destination
- **Click trend chart** — bar chart using Canvas API (no external libs)
- **Live refresh** — auto-updates every 5 seconds

---

## Phase 4 — URL History + QR Code

**File:** `frontend/history.html` + `frontend/js/history.js`

### What's included:
- **History table** — all shortened URLs with creation time, TTL, click count
- **Expiry indicator** — green (active) / red (expired) badge
- **QR Code modal** — click any URL to see its QR code (generated via Canvas)
- **Delete button** — remove URLs from history
- **Search/filter** — filter history by short code or destination

---

## Project Structure

```
frontend/
├── index.html          # Phase 1 — Landing page
├── shorten.html        # Phase 2 — URL shortener form
├── analytics.html      # Phase 3 — Analytics dashboard
├── history.html        # Phase 4 — URL history + QR
├── css/
│   └── style.css       # Shared design system
└── js/
    ├── shorten.js      # Phase 2 logic
    ├── analytics.js    # Phase 3 logic
    └── history.js      # Phase 4 logic
```

---

## Running the Frontend

Simply open any HTML file in a browser — no build step required:

```bash
# Open landing page
start frontend/index.html

# Or use a local server for best experience
npx serve frontend/
```

---

## Design System

| Token | Value |
|-------|-------|
| Primary | `#7c3aed` (purple) |
| Accent | `#06b6d4` (cyan) |
| Background | `#0f0f1a` (dark navy) |
| Surface | `rgba(255,255,255,0.05)` (glass) |
| Font | Inter (Google Fonts) |
| Border radius | `12px` |
| Animation | `0.3s ease` transitions |
