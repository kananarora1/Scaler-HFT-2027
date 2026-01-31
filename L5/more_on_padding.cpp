
#include <iostream>


struct String{
    char ptr[1021];
    uint32_t size;
};
struct Order{
   String *orderId;
   uint32_t qty;
   uint32_t price;
};
int main(){
  std::string a = "hedfshksjdfhdfsjkhgdfsjkgdhjdgshjdsaghjasdgasdjhkgaSDHJKDASGFhJADSFJKHADSfADSJKHfdashkjadllo";
  std::cout << sizeof(a);
  return 0;
}
