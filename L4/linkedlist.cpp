#include <iostream>
using namespace std;

struct Node{
	int data{0};
	Node* next{nullptr};
	~Node(){
		cout <<data <<"getting deleted" << endl;
	}
	Node(const Node& other) : data(other.data), next(other.next) {
    		std::cout << "deep copy of Node " << data << std::endl;
	}
};
Node makeLL(){
	Node head;
	head.data = 10;
	Node next;
	next.data = 11;
	head.next = &next;
	return head;
}
int main(){
	Node h = makeLL();
	while(h.next != nullptr){
		cout << h.data << endl;
		h = *h.next;
	}
	return 0;
}
