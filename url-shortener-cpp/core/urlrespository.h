#ifndef URL_REPOSITORY_H
#define URL_REPOSITORY_H

#include <unordered_map>
#include <string>

class UrlRepository {
private:
    std::unordered_map<std::string, std::string> store;

public:
    void save(const std::string& shortCode,
              const std::string& longUrl);

    std::string find(const std::string& shortCode);
};

#endif
