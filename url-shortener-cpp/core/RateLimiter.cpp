#include "RateLimiter.h"

RateLimiter::RateLimiter(double maxTok, double refRate)
    : maxTokens(maxTok), refillRate(refRate) {}

void RateLimiter::refill(Bucket& b) {
    auto now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - b.lastRefill).count();
    b.tokens = std::min(maxTokens, b.tokens + elapsed * refillRate);
    b.lastRefill = now;
}

bool RateLimiter::allowRequest(const std::string& ip) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = buckets.find(ip);
    if (it == buckets.end()) {
        // New IP: start with full bucket
        buckets[ip] = { maxTokens - 1.0, std::chrono::steady_clock::now() };
        return true;
    }

    Bucket& b = it->second;
    refill(b);

    if (b.tokens >= 1.0) {
        b.tokens -= 1.0;
        return true;
    }
    return false; // Rate limited
}

double RateLimiter::getTokens(const std::string& ip) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = buckets.find(ip);
    if (it == buckets.end()) return maxTokens;
    refill(it->second);
    return it->second.tokens;
}
