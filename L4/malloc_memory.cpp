#include <iostream>
#include <cstdlib>

int main() {
    // This looks simple:
    int* arr1 = new int[100];
    int* arr2 = new int[200];
    delete[] arr1;
    delete[] arr2;
    
    // But behind the scenes:
    // 1. new calls malloc(400 bytes) for arr1
    // 2. malloc might need 1 system call to get memory
    // 3. new calls malloc(800 bytes) for arr2  
    // 4. malloc might reuse memory or need another system call
    // 5. delete calls free() - no system calls!
}
