#include <iostream>
#include <chrono>
using namespace std;

class Animal {
public:
    virtual void makeSound() = 0;
    virtual void eat() = 0;
    virtual ~Animal() {} // Always add virtual destructor for base classes
};

class Tiger : public Animal {
public:
    void makeSound() override {
        cout << "Tiger is making sound" << endl;
    }
    void eat() override {
        cout << "Tiger is eating" << endl;
    }
};

class Duck : public Animal {
public:
    void makeSound() override {
        cout << "Duck is making sound" << endl;
    }
    void eat() override {
        cout << "Duck is eating" << endl;
    }
};

int main() {
    // Start timing
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; ++i) {
        Animal* tobj = new Tiger();
        tobj->makeSound();
        tobj->eat();
        delete tobj; // Free memory

        Animal* dobj = new Duck();
        dobj->makeSound();
        dobj->eat();
        delete dobj; // Free memory
    }

    // End timing
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, micro> elapsed = end - start;

    cout << "Time taken: " << elapsed.count() << " microseconds" << endl;

    return 0;
}

