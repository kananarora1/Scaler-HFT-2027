#include <iostream>
#include <thread>
#include <chrono>

int main() {
    for (int i = 0; i < 10000; i++) {  // run 50 iterations
        int* leak = new int[256 * 1024]; // ~1 MB

        // Force-touch memory so OS actually commits it
        leak[0] = 42;
        leak[128] = 99;
        leak[256 * 1024 - 1] = i;

        // Leak: we never delete[]
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::cout << "Done leaking!\n";
    std::this_thread::sleep_for(std::chrono::seconds(5)); // so you can check with htop
    return 0;
}

