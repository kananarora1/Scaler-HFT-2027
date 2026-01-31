#include <iostream>
#include <chrono>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

struct MarketData {
    uint64_t timestamp;
    double price;
    uint32_t volume;
};

// Parsing function (works on raw bytes, no extra allocations)
inline MarketData parse(const char* buffer) {
    MarketData data;
    std::memcpy(&data, buffer, sizeof(MarketData));
    return data;
}

int main() {
    char buffer[sizeof(MarketData)];
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // Assume already connected...

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; i++) {
        read(sock, buffer, sizeof(buffer));
        MarketData md = parse(buffer);
        // Decision logic here (fast math, no heap allocation)
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Elapsed: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << " us\n";

    close(sock);
    return 0;
}

