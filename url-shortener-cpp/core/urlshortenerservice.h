#ifndef URL_SHORTENER_SERVICE_H
#define URL_SHORTENER_SERVICE_H

#include "LRUCache.h"
#include "Idgenerator.h"
#include "urlrespository.h"
#include "RateLimiter.h"
#include "AnalyticsTracker.h"
#include "consistenthashing.h"
#include <string>
#include <vector>

// Main orchestrator — coordinates all components
class UrlShortenerService {
private:
    Idgenerator     idgenerator;   // Unique ID generation
    LRUCache        cache;         // In-memory LRU cache
    UrlRepository   repository;    // Persistent storage (with TTL)
    RateLimiter     rateLimiter;   // Token bucket per IP
    AnalyticsTracker analytics;    // Click tracking
    ConsistentHashRing hashRing;   // Distributed sharding

public:
    // cache size = 100, rate limit = 5 req burst / 2 per sec
    UrlShortenerService();

    // Shorten a URL
    // ttlSeconds = 0 means no expiry
    // ip = "" means no rate limiting
    // customAlias = "" means auto-generate short code
    std::string shortenUrl(const std::string& longUrl,
                           int ttlSeconds = 0,
                           const std::string& ip = "",
                           const std::string& customAlias = "");

    // Redirect short code → long URL (with cache + analytics)
    // ip = "" means no rate limiting
    std::string redirect(const std::string& shortCode,
                         const std::string& ip = "");

    // Print analytics report
    void printAnalytics(int topN = 10);

    // Print which hash-ring node a short code maps to
    void printNodeAssignment(const std::string& shortCode);

    // Add a storage node to the consistent hash ring
    void addNode(int nodeId);
};

#endif
