#ifndef URL_REPOSITORY_H
#define URL_REPOSITORY_H

#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>

// Stores URL mappings with optional TTL expiry
struct UrlEntry {
    std::string longUrl;
    std::chrono::steady_clock::time_point expiresAt;
    bool hasExpiry;
};

class UrlRepository {
private:
    std::unordered_map<std::string, UrlEntry> store;
    mutable std::mutex mtx;

public:
    // Save URL with optional TTL in seconds (0 = no expiry)
    void save(const std::string& shortCode,
              const std::string& longUrl,
              int ttlSeconds = 0);

    // Find URL by short code; returns "" if not found or expired
    std::string find(const std::string& shortCode);

    // Check if a short code exists (for custom alias validation)
    bool exists(const std::string& shortCode);

    // Remove a specific entry
    void remove(const std::string& shortCode);
};

#endif
