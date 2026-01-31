#include <iostream>
#include  <vector>

using namespace std;
enum Side { BUY, SELL};
class Order{
public:
  int32_t id;
  Side side;
  virtual void cancel(){
   	std::cout << "IDK\n";	
  }
};

class Ioc: public Order{
public:
    void cancel(){
	std::cout << "Not Supported\n";
   }
};

class Gtc: public Order{
public:
   int32_t timeStamp;
   void cancel(){
       std::cout << "Success\n";
   }
};
int main(){
  Gtc gtc;
  Order *o = &gtc;
  o->cancel();
}

