#include <iostream>
using namespace std;


struct Position{  std::string id;};
struct Order{     std::string id;};


/// A FUNCTION WHILL FILLS CURRENT TIMESTAMP TO IT
template<typename T>
void modifyNameOfClass(T obj){
	obj.id+= "KARTIK";
}


// CLASS VECTOR OF THINGS 
int main() {

   Position a{"pos"};
   Order    b{"pos"};
   modifyNameOfClass(a);
   modifyNameOfClass(b);
   std::cout << a.id << endl;
   std::cout << b.id << endl;

}
