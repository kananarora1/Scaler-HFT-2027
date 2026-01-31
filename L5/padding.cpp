#include <iostream>
#include <cstdint>
#include <chrono>
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <vector>
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


BOX1 .....  0 TO 15 
BOX2 ...... 16 TO 31
CHAR ....... 32
BOX3 ....... 33
struct Box{
  uint64_t size{1u<<50};
  uint64_t length{1u<<55};
};
int main(){
	std::srand(std::time(nullptr));
	auto start = std::chrono::high_resolution_clock::now();
	MemoryPool* mPool = new MemoryPool();	
	vector<Box*> ptrs;
	for(int i=0;i<10000000;i++){
	        Box* box1 = reinterpret_cast<Box*>(mPool->getMemory(sizeof(Box)));
        	new (box1) Box();
		char * ch = (char*)mPool->getMemory(1);
        	*ch = 'k';
		Box* box2 = reinterpret_cast<Box*>(mPool->getMemory(sizeof(Box)));
        	new (box2) Box();
		ptrs.push_back(box1);
		ptrs.push_back(box2);
	}
	int trials = 1000000000;
        while(trials){
		int randIdx = rand();
		randIdx     = randIdx % 1000;
		ptrs[randIdx] ->size += 1u<<50;
		trials--;
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
    	std::cout << " elapsed: " << elapsed.count() << std::endl;
	//std::cout << end - start << std::endl;

}

