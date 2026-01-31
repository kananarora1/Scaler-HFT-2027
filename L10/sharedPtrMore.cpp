#include <iostream>
#include <memory>   // for std::shared_ptr

class Cat {
public:
    Cat(const std::string& name) : name(name) {
        std::cout << "Cat " << name << " constructed\n";
    }
    ~Cat() {
        std::cout << "Cat " << name << " destroyed\n";
    }
    void meow() {
        std::cout << name << " says: Meow!\n";
    }
private:
    std::string name;
};

int main() {
    // Create a shared_ptr
    std::shared_ptr<Cat> c1 = std::make_shared<Cat>("Kitty");
    {
        std::shared_ptr<Cat> c2 = c1;  // another shared_ptr points to same Cat
        std::cout << "Use count = " << c1.use_count() << "\n"; // 2
        c2->meow();
    } // c2 goes out of scope, but object not destroyed yet

    std::cout << "After c2 is gone, use count = " << c1.use_count() << "\n"; // 1
    c1->meow();

    // Automatic cleanup when last shared_ptr (c1) goes out of scope
    return 0;
}

