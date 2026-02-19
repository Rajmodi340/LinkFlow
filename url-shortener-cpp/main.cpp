#include <iostream>
#include <thread>
#include <chrono>
#include "core/urlshortenerservice.h"
#include "core/QRCodeStub.h"

// Helper: print a section header
void section(const std::string& title) {
    std::cout << "\n";
    std::cout << "══════════════════════════════════════════════\n";
    std::cout << "  " << title << "\n";
    std::cout << "══════════════════════════════════════════════\n";
}

int main() {
    UrlShortenerService service;

    // ─────────────────────────────────────────────
    // PHASE 1: Basic URL Shortening & Redirect
    // ─────────────────────────────────────────────
    section("PHASE 1 — Basic Shorten & Redirect");

    std::string google = service.shortenUrl("https://google.com");
    std::string github = service.shortenUrl("https://github.com");
    std::string openai = service.shortenUrl("https://openai.com");

    std::cout << "  ✅ google.com  → short.url/" << google << "\n";
    std::cout << "  ✅ github.com  → short.url/" << github << "\n";
    std::cout << "  ✅ openai.com  → short.url/" << openai << "\n";

    std::cout << "\n  Redirect '" << google << "' → " << service.redirect(google) << "\n";
    std::cout << "  Redirect '" << github << "' → " << service.redirect(github) << "\n";

    // ─────────────────────────────────────────────
    // PHASE 1: LRU Cache Demo
    // ─────────────────────────────────────────────
    section("PHASE 1 — LRU Cache (10 rapid redirects)");

    for (int i = 0; i < 10; i++) {
        std::string result = service.redirect(google);
        std::cout << "  Hit #" << (i + 1) << " → " << result << "\n";
    }

    // ─────────────────────────────────────────────
    // PHASE 2: Rate Limiting
    // ─────────────────────────────────────────────
    section("PHASE 2 — Rate Limiting (Token Bucket, limit=5 burst)");

    std::string myIp = "192.168.1.100";
    std::string testUrl = service.shortenUrl("https://example.com");

    std::cout << "  Sending 7 requests from IP: " << myIp << "\n\n";
    for (int i = 1; i <= 7; i++) {
        std::string result = service.redirect(testUrl, myIp);
        if (!result.empty()) {
            std::cout << "  Request #" << i << " ✅ → " << result << "\n";
        } else {
            std::cout << "  Request #" << i << " ❌ BLOCKED by rate limiter\n";
        }
    }

    // ─────────────────────────────────────────────
    // PHASE 2: URL Expiry (TTL)
    // ─────────────────────────────────────────────
    section("PHASE 2 — URL Expiry (TTL = 2 seconds)");

    std::string tempUrl = service.shortenUrl("https://temporary-promo.com", 2 /*ttl=2s*/);
    std::cout << "  Created: short.url/" << tempUrl << " (expires in 2s)\n";

    std::string res1 = service.redirect(tempUrl);
    std::cout << "  Redirect immediately → " << (res1.empty() ? "NOT FOUND" : res1) << "\n";

    std::cout << "  ⏳ Waiting 3 seconds...\n";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::string res2 = service.redirect(tempUrl);
    std::cout << "  Redirect after expiry → " << (res2.empty() ? "❌ EXPIRED (as expected)" : res2) << "\n";

    // ─────────────────────────────────────────────
    // PHASE 3: Consistent Hashing
    // ─────────────────────────────────────────────
    section("PHASE 3 — Consistent Hashing (3 storage nodes)");

    service.addNode(1);
    service.addNode(2);
    service.addNode(3);

    std::cout << "  Node assignments for short codes:\n";
    service.printNodeAssignment(google);
    service.printNodeAssignment(github);
    service.printNodeAssignment(openai);
    service.printNodeAssignment(testUrl);

    // ─────────────────────────────────────────────
    // PHASE 4: Custom Alias
    // ─────────────────────────────────────────────
    section("PHASE 4 — Custom Alias Support");

    std::string alias1 = service.shortenUrl("https://linkedin.com/in/akshay", 0, "", "akshay");
    std::string alias2 = service.shortenUrl("https://myportfolio.dev", 0, "", "portfolio");
    // Try duplicate alias
    std::string alias3 = service.shortenUrl("https://other.com", 0, "", "akshay");

    std::cout << "  ✅ Custom alias 'akshay'    → " << service.redirect("akshay") << "\n";
    std::cout << "  ✅ Custom alias 'portfolio' → " << service.redirect("portfolio") << "\n";
    std::cout << "  ↳ Duplicate 'akshay' result: " << (alias3.empty() ? "blocked (alias taken)" : alias3) << "\n";

    // ─────────────────────────────────────────────
    // PHASE 4: QR Code Generation (ASCII stub)
    // ─────────────────────────────────────────────
    section("PHASE 4 — QR Code (ASCII Stub)");

    std::cout << "  QR for 'akshay':\n";
    QRCodeStub::printQR("akshay");
    std::cout << "  QR for '" << google << "':\n";
    QRCodeStub::printQR(google);

    // ─────────────────────────────────────────────
    // PHASE 4: Analytics Dashboard
    // ─────────────────────────────────────────────
    section("PHASE 4 — Analytics Dashboard");

    // Generate some traffic
    for (int i = 0; i < 15; i++) service.redirect(google);
    for (int i = 0; i < 8;  i++) service.redirect(github);
    for (int i = 0; i < 3;  i++) service.redirect(openai);
    for (int i = 0; i < 5;  i++) service.redirect("akshay");
    for (int i = 0; i < 2;  i++) service.redirect("portfolio");

    service.printAnalytics(5);

    std::cout << "\n🎉 All phases complete!\n\n";
    return 0;
}
