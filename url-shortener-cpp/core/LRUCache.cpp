#include "LRUCache.h"

LRUCache::LRUCache(int cap) : capacity(cap) {}

bool LRUCache::get(const std::string& key, std::string& value) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = cache.find(key);
    if (it == cache.end()) return false;

    value = it->second.first;
    order.erase(it->second.second);
    order.push_front(key);
    it->second.second = order.begin();
    return true;
}

void LRUCache::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = cache.find(key);

    if (it != cache.end()) {
        order.erase(it->second.second);
        order.push_front(key);
        it->second = {value, order.begin()};
        return;
    }

    if ((int)cache.size() == capacity) {
        std::string lruKey = order.back();
        order.pop_back();
        cache.erase(lruKey);
    }

    order.push_front(key);
    cache[key] = {value, order.begin()};
}

void LRUCache::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = cache.find(key);
    if (it != cache.end()) {
        order.erase(it->second.second);
        cache.erase(it);
    }
}

int LRUCache::size() const {
    std::lock_guard<std::mutex> lock(mtx);
    return (int)cache.size();
}
