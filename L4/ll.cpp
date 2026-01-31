#include <iostream>
using namespace std;
struct Node {
    int data{0};
    Node* next{nullptr};

    Node() = default;

    // Shallow copy constructor (to demonstrate dangling pointer)
    Node(const Node& other) : data(other.data), next(other.next) {
        std::cout << "copy of Node " << data << std::endl;
    }

    // Destructor (don’t delete next, just print)
    ~Node() {
        std::cout << data << " getting deleted" << std::endl;
    }
};
Node makeLL(){ Node head; head.data = 10; Node next; next.data = 11; head.next = &next; return head; } int main(){ Node h = makeLL(); while(h.next != nullptr){ cout << h.data << endl; h = *h.next; } return 0; }
