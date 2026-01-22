#include <iostream>
#include "core/urlshortenerservice.h"

int main() {

    UrlShortenerService service;
    // service ka object banaya

    std::string short1 = service.shortenUrl("https://google.com");
    // pehla URL shorten kiya

    std::cout << "Short code: " << short1 << std::endl;
    std::cout << "Redirect: "
              << service.redirect(short1) << std::endl;

    std::string short2 = service.shortenUrl("https://openai.com");

    std::cout << "Short code: " << short2 << std::endl;
    std::cout << "Redirect: "
              << service.redirect(short2) << std::endl;

    return 0;
}
