#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <string>
#include <unordered_map>
#include <chrono>
#include <mutex>

// Token Bucket Rate Limiter (per IP)
class RateLimiter {
private:
    struct Bucket {
        double tokens;
        std::chrono::steady_clock::time_point lastRefill;
    };

    double maxTokens;    // max burst capacity
    double refillRate;   // tokens added per second
    std::unordered_map<std::string, Bucket> buckets;
    std::mutex mtx;

    void refill(Bucket& b);

public:
    // maxTokens = burst limit, refillRate = tokens/second
    RateLimiter(double maxTokens = 5.0, double refillRate = 2.0);

    // Returns true if request is allowed, false if rate-limited
    bool allowRequest(const std::string& ip);

    // Get remaining tokens for an IP (for display)
    double getTokens(const std::string& ip);
};

#endif
