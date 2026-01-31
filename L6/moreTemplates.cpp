#include <iostream>
using namespace std;

struct Student{
	std::string id;
	std::string name;
	//
};

struct order{
	std::string id;
	std::string food;
};

template<typename T>
void appendHello(T obj){
	obj.id += "HELLO";
}
int main(){
	Student s{"1","Naman"};
	Order   o{"order1","Burger"};
	appendHello(s);
	appendHello(o);
	std::cout << s.id << endl;
	std::cout << o.id << endl;
}
