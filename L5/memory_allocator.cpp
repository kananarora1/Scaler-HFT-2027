#include <iostream>
#include <cstdint>
using namespace std;


// CHUNK
// BUFFER  0 1 2 3 4 5 6 7 8 9 10 11 12
// ONE BOX
// STARTING INDEX ... ENDING INDEX
// BOX 1  0          15
// BOX 2  16 ........31



const uint64_t SIZE = 1024 * 1024 * 1024;
struct MemoryPool{
   char buffer[SIZE];
   uint64_t offset{0};
   void* getMemory(uint64_t bytesNeeded){
	if(offset + bytesNeeded > SIZE)
		return nullptr;

	void* ptrToOffset = reinterpret_cast<void*>(&(buffer[offset]));
        offset += bytesNeeded;
	return ptrToOffset;
   }
};

struct Box{
  uint64_t size;
  uint64_t length;
};

int main(){
	MemoryPool* mPool = new MemoryPool();	
	for(int i=0;i<100;i++){
		Box* box1 = reinterpret_cast<Box*>(mPool->getMemory(sizeof(Box)));
        	box1->size = 10;
        	cout << box1->size << endl;
	}
}
