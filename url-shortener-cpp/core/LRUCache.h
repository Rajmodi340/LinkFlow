#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <unordered_map>
#include <list>
#include <string>
#include <mutex>

class LRUCache {
private:
    int capacity;
    std::list<std::string> order;
    std::unordered_map<
        std::string,
        std::pair<std::string, std::list<std::string>::iterator>
    > cache;
    mutable std::mutex mtx;

public:
    LRUCache(int cap);

    // Returns true and fills value if key found; false otherwise
    bool get(const std::string& key, std::string& value);

    // Insert or update key-value pair; evicts LRU if at capacity
    void put(const std::string& key, const std::string& value);

    // Remove a key (used when URL expires)
    void remove(const std::string& key);

    // Current number of cached entries
    int size() const;
};

#endif