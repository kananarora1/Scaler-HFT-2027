#include <iostream>
#include <cstdint>
using namespace std;

struct X{
  uint32_t x;  // 4 
  uint32_t y;  // 4
  char     z;  // 1
  // 3 extra bytes
};

ALIGNMENT OF A STRUCT .. WHICH ADDRESS IT SHOULD START FROM
/// MAXIMUM ALIGNMENT OF ATTRIBUTES
struct Trick2{
	char a;
	int32_t b;
        char c;
};

a   b b b b c a     b b b  c a     b b b

////   
/// 0  1  2  3   4   5   6   7    8    9     10    11    12

//  a             b   b   b   b    c                      a


// 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
// x x x x y y y y z x  x  x  x x  x   x

int main(){
	std::cout << sizeof(X);
}
