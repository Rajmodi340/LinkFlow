#include <iostream>
#include "core/UrlShortenerService.h"

int main() {

    UrlShortenerService service;

    // Step 1: create URLs
    std::string google = service.shortenUrl("https://google.com");
    std::string openai = service.shortenUrl("https://openai.com");
    std::string github = service.shortenUrl("https://github.com");

    // Step 2: simulate heavy redirects
    for (int i = 0; i < 10; i++) {
        std::cout << "Redirect Google: "
                  << service.redirect(google) << std::endl;
    }

    return 0;
}
