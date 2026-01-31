#include<iostream>

using namespace std;

void changeX(int *x){
	x = nullptr;
}
int main(){
	int x = 12;
	changeX(&x);
	cout << x << endl;
	return 0;
}
