#include <iostream>
using namespace std;

void changeVar(int* ptrToX){
	*ptrToX = 13;
}
int main(){
     int x = 12;
     changeVar(&x);
     cout << x << endl;
     return 0;
}
