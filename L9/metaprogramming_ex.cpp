#include <iostream>

// Primary template
template <int N>
struct Factorial {
    static const int value = N * Factorial<N - 1>::value;
};

// Base case
template <>
struct Factorial<0> {
    static const int value = 1;
};

int main() {
    std::cout << "Factorial<5> = " << Factorial<5>::value << "\n"; // prints 120
}

