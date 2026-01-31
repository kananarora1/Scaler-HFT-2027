#include <iostream>
using namespace std;

struct Order{
   int32_t orderId;
};
int main(){
   unsigned char buffer[4];
   buffer[0] = 0b00000000;
   buffer[1] = 0b00000010;
   buffer[2] = 0b00000000;
   buffer[3] = 0b00000000;
   Order* ptr = reinterpret_cast<Order*>(buffer);
   std::cout << ptr->orderId <<endl;
}
