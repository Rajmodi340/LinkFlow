#include "urlrespository.h"

void UrlRepository::save(const std::string& shortCode,
                         const std::string& longUrl) {
    store[shortCode] = longUrl;
}

// find shortCode
std::string UrlRepository::find(const std::string& shortCode) {
    if (store.find(shortCode) != store.end()) {
        return store[shortCode];
    }
    return "";
}
