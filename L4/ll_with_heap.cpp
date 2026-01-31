#include <iostream>
#include <cmath>
using namespace std; 

struct ListNode {
	int data;
	ListNode* next;
	ListNode(int data,ListNode* next){
		this->data = data;
		this->next = next;
	}	
	~ListNode(){
		cout << "Node with data " << data << " getting deleted" ;
	}	 
}; 

ListNode* createLinkedList() {
	ListNode* head = new ListNode(0,nullptr);
	ListNode* head2 = head;
	for (int i = 1; i < 1000;i++){
		head2->next = new ListNode(i,nullptr);
				head2 = head2->next;
			int arr[1024*1024];	
			arr[464837] = 122;
			arr[4336] = 323;
			cout << arr[4678] << "\n";	
}
	return head; 
}
int main()
{
	ListNode* head = createLinkedList();
	while(head!=nullptr){
		cout << head->data << " "; head=head->next; 
	} return 0;
}

