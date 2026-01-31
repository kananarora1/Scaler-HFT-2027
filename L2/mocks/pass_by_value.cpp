#include <iostream>


struct Order{
   int orderId;
   int price;
   int qty;
   int filledQty;
};
void sendOrderToExchange(){

}
int main(){
   Order order{1,500,10};  // calling constructor by default ..	
   sendOrderToExchange()
}
