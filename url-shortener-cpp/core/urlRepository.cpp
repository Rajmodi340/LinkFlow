#include "urlrespository.h"

void UrlRepository::save(const std::string& shortCode,
                         const std::string& longUrl,
                         int ttlSeconds) {
    std::lock_guard<std::mutex> lock(mtx);
    UrlEntry entry;
    entry.longUrl = longUrl;
    entry.hasExpiry = (ttlSeconds > 0);
    if (entry.hasExpiry) {
        entry.expiresAt = std::chrono::steady_clock::now()
                        + std::chrono::seconds(ttlSeconds);
    }
    store[shortCode] = entry;
}

std::string UrlRepository::find(const std::string& shortCode) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = store.find(shortCode);
    if (it == store.end()) return "";

    const UrlEntry& entry = it->second;
    if (entry.hasExpiry) {
        auto now = std::chrono::steady_clock::now();
        if (now > entry.expiresAt) {
            // URL has expired — remove it
            store.erase(it);
            return "";
        }
    }
    return entry.longUrl;
}

bool UrlRepository::exists(const std::string& shortCode) {
    std::lock_guard<std::mutex> lock(mtx);
    return store.find(shortCode) != store.end();
}

void UrlRepository::remove(const std::string& shortCode) {
    std::lock_guard<std::mutex> lock(mtx);
    store.erase(shortCode);
}
