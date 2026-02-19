#include "urlshortenerservice.h"
#include "Base62Encoder.h"
#include <iostream>

UrlShortenerService::UrlShortenerService()
    : cache(100),
      rateLimiter(5.0, 2.0),
      hashRing(3)
{}

std::string UrlShortenerService::shortenUrl(const std::string& longUrl,
                                             int ttlSeconds,
                                             const std::string& ip,
                                             const std::string& customAlias) {
    // Rate limiting check
    if (!ip.empty() && !rateLimiter.allowRequest(ip)) {
        std::cout << "  ⛔ Rate limit exceeded for IP: " << ip << "\n";
        return "";
    }

    std::string shortCode;

    if (!customAlias.empty()) {
        // Custom alias: check it's not already taken
        if (repository.exists(customAlias)) {
            std::cout << "  ⚠️  Alias '" << customAlias << "' already in use.\n";
            return "";
        }
        shortCode = customAlias;
    } else {
        // Auto-generate: ID → Base62
        long long id = idgenerator.getNextId();
        shortCode = Base62Encoder::encode(id);
    }

    // Save to repository (with optional TTL)
    repository.save(shortCode, longUrl, ttlSeconds);

    return shortCode;
}

std::string UrlShortenerService::redirect(const std::string& shortCode,
                                           const std::string& ip) {
    // Rate limiting check
    if (!ip.empty() && !rateLimiter.allowRequest(ip)) {
        std::cout << "  ⛔ Rate limit exceeded for IP: " << ip << "\n";
        return "";
    }

    std::string longUrl;

    // 1. Check LRU cache first
    if (cache.get(shortCode, longUrl)) {
        // Cache hit
        analytics.recordHit(shortCode);
        return longUrl;
    }

    // 2. Cache miss — fetch from repository
    longUrl = repository.find(shortCode);

    if (longUrl.empty()) {
        return ""; // Not found or expired
    }

    // 3. Warm the cache
    cache.put(shortCode, longUrl);

    // 4. Record analytics
    analytics.recordHit(shortCode);

    return longUrl;
}

void UrlShortenerService::printAnalytics(int topN) {
    analytics.printReport(topN);
}

void UrlShortenerService::printNodeAssignment(const std::string& shortCode) {
    int node = hashRing.getNode(shortCode);
    std::cout << "  🔗 '" << shortCode << "' → Node " << node << "\n";
}

void UrlShortenerService::addNode(int nodeId) {
    hashRing.addNode(nodeId);
}