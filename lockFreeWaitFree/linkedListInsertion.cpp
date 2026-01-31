#include <atomic>
#include <iostream>
#include <thread>

struct Node {
    int value;
    Node* next; // raw pointer is fine here
    Node(int v) : value(v), next(nullptr) {}
};

class LockFreeList {
    std::atomic<Node*> head;

public:
    LockFreeList() : head(nullptr) {}

    void insert(int val) {
        Node* newNode = new Node(val);
        Node* oldHead;

        // try until CAS succeeds
        do {
            oldHead = head.load();
            newNode->next = oldHead;
        } while (!head.compare_exchange_strong(oldHead, newNode));
    }

    void print() {
        Node* curr = head.load();
        while (curr) {
            std::cout << curr->value << " ";
            curr = curr->next;
        }
        std::cout << "\n";
    }
};

int main() {
    LockFreeList list;

    std::thread t1([&]() {
        for (int i = 1; i <= 5; i++) list.insert(i * 10);
    });

    std::thread t2([&]() {
        for (int i = 1; i <= 5; i++) list.insert(i * 100);
    });

    t1.join();
    t2.join();

    list.print();
}

