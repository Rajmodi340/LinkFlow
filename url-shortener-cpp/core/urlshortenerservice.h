#ifndef URL_SHORTENER_SERVICE_H
#define URL_SHORTENER_SERVICE_H



#include <string>
#include "Idgenerator.h"
#include "urlrespository.h"



// Ye class system ka BRAIN hai
class UrlShortenerService {
private:
    Idgenerator idgenerator;
    // unique ID generate karega

    UrlRepository repository;
    // shortCode → longURL store karega

public:
    std::string shortenUrl(const std::string& longUrl);
    // long URL ko short code me convert karega

    std::string redirect(const std::string& shortCode);
    // short code se long URL dega
};

#endif