#include <iostream>
#include <vector>
#include <chrono>

struct Point { int x, y; Point(int a,int b){ x = a; y = b;} };

int main() {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    std::vector<Point> arr;
    arr.reserve(10000000);
    for (int i = 0; i < 10000000; ++i) {
        arr.emplace_back(i, i);
    }
    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start).count();
    std::cout << "C++: Created " << arr.size() << " Points in " << duration << " µs\n";
    return 0;
}

