#include <iostream>
#include <memory>   // for std::unique_ptr

class Dog {
public:
    Dog(const std::string& name) : name(name) {
        std::cout << "Dog " << name << " constructed\n";
    }
    ~Dog() {
        std::cout << "Dog " << name << " destroyed\n";
    }
    void bark() {
        std::cout << name << " says: Woof!\n";
    }
private:
    std::string name;
};

int main() {
    // Create a unique_ptr
    std::unique_ptr<Dog> d1 = std::make_unique<Dog>("Buddy");
    d1->bark();

    // Transfer ownership (move)
    std::unique_ptr<Dog> d2 = std::move(d1);

    if (!d1) {
        std::cout << "d1 is now empty (ownership moved)\n";
    }
    d2->bark();

    // Automatic cleanup when d2 goes out of scope
    return 0;
}

