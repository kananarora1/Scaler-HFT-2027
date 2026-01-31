#include<iostream>

using namespace std;

//  student
// sst  academy

struct SST{
 std::string name{"Aditya"};
 int marks{-10};
};


struct Academy{
  int marks{100};
};

template<typename T>
int func(T student){
  return student.marks;
}

int main(){

  SST obj1;
  Academy obj2;
  cout << func<SST>(obj1) << endl;
  cout << func<Academy>(obj2) << endl;
  return 0;
}

