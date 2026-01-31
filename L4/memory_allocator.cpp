#include <iostream>
#include <new>        // placement new
#include <cstdint>
#include <cstring>    // memset

struct Block {
    unsigned char *buffer;  // raw storage
    size_t used = 0;                 // how many bytes consumed
};

struct Order {
    int id;
    double price;
    char tag[16];
};

int main() {
    for(int i=0;i<100000;i++){
        int* arr = new int[100];
    }
    Block arena;
    arena.buffer = new unsigned char[1024*1024*1024];
    void* rawPtr = arena.buffer + arena.used;
    arena.used += sizeof(Order);
    Order* o1 = reinterpret_cast<Order*>(rawPtr);

    // Now actually construct an Order object in that spot (placement new)
    new (o1) Order{42, 123.45, "apple"};  // runs constructor

    // std::cout << "Order: id=" << o1->id
    //           << " price=" << o1->price
    //           << " tag=" << o1->tag << "\n";

    void* rawPtr2 = arena.buffer + arena.used;
    arena.used += sizeof(Order);

    Order* o2 = reinterpret_cast<Order*>(rawPtr2);
    new (o2) Order{99, 555.55, "banana"};

    // std::cout << "Order2: id=" << o2->id
    //           << " price=" << o2->price
    //           << " tag=" << o2->tag << "\n";

    // IMPORTANT: destroy objects manually when done
    o1->~Order();
    o2->~Order();
}
