#include "AnalyticsTracker.h"

void AnalyticsTracker::recordHit(const std::string& shortCode) {
    std::lock_guard<std::mutex> lock(mtx);
    hitCounts[shortCode]++;
}

long long AnalyticsTracker::getHitCount(const std::string& shortCode) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = hitCounts.find(shortCode);
    if (it == hitCounts.end()) return 0;
    return it->second;
}

void AnalyticsTracker::printReport(int topN) {
    std::lock_guard<std::mutex> lock(mtx);

    // Copy to vector for sorting
    std::vector<std::pair<std::string, long long>> entries(hitCounts.begin(), hitCounts.end());
    std::sort(entries.begin(), entries.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::cout << "\n📊 Analytics Report (Top " << topN << " URLs):\n";
    std::cout << "  ┌─────────────┬───────────┐\n";
    std::cout << "  │  Short Code │   Clicks  │\n";
    std::cout << "  ├─────────────┼───────────┤\n";

    int count = 0;
    for (const auto& [code, hits] : entries) {
        if (count++ >= topN) break;
        std::cout << "  │ " << std::left;
        // Pad short code to 11 chars
        std::string padded = code;
        while (padded.size() < 11) padded += ' ';
        std::cout << padded << " │ " << std::right;
        std::string hitsStr = std::to_string(hits);
        while (hitsStr.size() < 9) hitsStr = " " + hitsStr;
        std::cout << hitsStr << " │\n";
    }
    std::cout << "  └─────────────┴───────────┘\n";
}
