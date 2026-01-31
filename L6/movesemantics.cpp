#include <iostream>

using namespace std;

/*
#OTHER 
#4 BYTES
#8 BYTES 
8 BYTES ------------------> [ 2 mb]
				
		         	
COPY		
4 BYTES			     		
8 BYTES                      	
8 BYTES--------------------------[ 2mb]

*/
// move semantics
struct DbConnection{
  int id;
  std::string connectionName;
  char buffer[2*1024*1024];  
  DbConnection(){
      id = 12;
      connectionName = "something";
      buffer = new char[2*1024*1024];
  }
  
  // COPY CONSTRUCTOR
  DbConnection(const DbConnection& other){
	this->id = other.id;
	this->connectionName = other.connectionName;
	//this->buffer = memcpy(other.buffer,this->buffer,2*1024*1024);    
  }
  
  // COPY ASSIGNMENT
  DbConnection& operator=(const DbConnection& other){
      //delete this->buffer
      this = DbConnection(other);
      return *this;
  }
};
DbConnection getDBconnection(){
    DbConnection db;
    return db;
}
int main(){
    DbConnection b = getDBconnection();

}
