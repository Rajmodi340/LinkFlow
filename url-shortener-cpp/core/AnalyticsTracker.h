#ifndef ANALYTICS_TRACKER_H
#define ANALYTICS_TRACKER_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <algorithm>
#include <iostream>

// Tracks click counts per short code
class AnalyticsTracker {
private:
    std::unordered_map<std::string, long long> hitCounts;
    std::mutex mtx;

public:
    // Record a redirect hit for a short code
    void recordHit(const std::string& shortCode);

    // Get total hits for a short code
    long long getHitCount(const std::string& shortCode);

    // Print top N URLs by click count
    void printReport(int topN = 10);
};

#endif
