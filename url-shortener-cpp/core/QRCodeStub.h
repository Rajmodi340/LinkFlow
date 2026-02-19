#ifndef QR_CODE_STUB_H
#define QR_CODE_STUB_H

#include <string>
#include <iostream>

// QR Code stub - prints ASCII art placeholder
// In production, integrate with a real QR library (e.g., libqrencode)
class QRCodeStub {
public:
    static void printQR(const std::string& shortCode, const std::string& baseUrl = "https://short.url/") {
        std::string fullUrl = baseUrl + shortCode;
        std::cout << "\n  ╔══════════════════╗\n";
        std::cout << "  ║ ▓▓  ░░░  ▓▓░░▓▓ ║\n";
        std::cout << "  ║ ▓▓  ░░░  ░░▓▓░░ ║\n";
        std::cout << "  ║ ░░  ▓▓▓  ▓▓░░▓▓ ║\n";
        std::cout << "  ║ ▓▓░░▓▓░  ░░▓▓░░ ║\n";
        std::cout << "  ║ ░░▓▓░░▓  ▓▓░░▓▓ ║\n";
        std::cout << "  ╚══════════════════╝\n";
        std::cout << "  QR → " << fullUrl << "\n\n";
    }
};

#endif
