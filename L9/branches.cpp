#include <iostream>

// Macros for readability
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

int main() {
    int x = 5;

    if (likely(x == 5)) {
        std::cout << "Fast path (likely true)\n";
    } else if (unlikely(x == 0)) {
        std::cout << "Unlikely branch\n";
    } else {
        std::cout << "Normal branch\n";
    }
}

