#include <iostream>
using namespace std;
int main(){
	for(int i = 0;i<1000;i++){
		int *arr = new int[1000*1000];
		arr[437] = 1;
		arr[37382] = 1;
	}
	return 0;
}
