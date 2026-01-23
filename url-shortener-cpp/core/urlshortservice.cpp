#include "urlshortenerservice.h"
#include "Base62Encoder.h"


UrlShortenerService::UrlShortenerService()
    : cache(3)   // cache size = 3 (example)
{
}

std::string UrlShortenerService::shortenUrl(const std::string& longUrl) {

    // step 1: unique ID lo
    long long id = idgenerator.getNextId();

    // step 2: ID ko Base62 me convert karo
    std::string shortCode = Base62Encoder::encode(id);

    // step 3: mapping save karo
    repository.save(shortCode, longUrl);

    // step 4: shortCode return karo
    return shortCode;
}

std::string UrlShortenerService::redirect(const std::string& shortCode) {

    std::string longUrl;
     if (cache.get(shortCode, longUrl)) {
        // cache hit
        return longUrl;
    }

      longUrl = repository.find(shortCode);
    // shortCode se longURL fetch karo
        // step 3: cache me daal do
    if (!longUrl.empty()) {
        cache.put(shortCode, longUrl);
    }

    return longUrl;
}