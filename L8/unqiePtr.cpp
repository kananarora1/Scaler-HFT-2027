#include <iostream>

// A very simple unique_ptr
template <typename T>
class UniquePtr {
    T* ptr;

public:
    // Constructor
    explicit UniquePtr(T* p = nullptr) : ptr(p) {}

    // Destructor
    ~UniquePtr() {
        delete ptr;
    }

    // Disable copy (unique ownership)
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Enable move
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;         // delete old resource
            ptr = other.ptr;    // take ownership
            other.ptr = nullptr;
        }
        return *this;
    }

    // Operators to access the object
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }

    // Release ownership
    T* release() {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    // Reset with a new pointer
    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }
};

// Example usage
struct Test {
    void hello() { std::cout << "Hello from Test!\n"; }
};

int main() {
    UniquePtr<Test> up1(new Test());   // create unique_ptr
    up1->hello();

    // Move ownership
    UniquePtr<Test> up2 = std::move(up1);
    if (!up1.get()) {
        std::cout << "up1 is empty after move\n";
    }
    up2->hello();

    // Reset pointer
    up2.reset(new Test());
    up2->hello();

    return 0;
}

