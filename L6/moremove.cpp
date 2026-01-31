#include <iostream>
#include <cstring>
using namespace std;

struct DbConnection{
  int id;
  std::string connectionName;
  char* buffer;
  DbConnection(){
      std::cout <<"DEFAULT constructor\n";
      id = 12;
      connectionName = "something";
      buffer = (char*) (malloc(2*1024*1024));
  }

  // COPY CONSTRUCTOR
  DbConnection(const DbConnection& other){
          std::cout <<"copy constructor\n";
        this->id = other.id;
        this->connectionName = other.connectionName;
        this->buffer = (char *) memcpy (this->buffer,other.buffer,2*1024*1024);
  }

  // COPY ASSIGNMENT
  DbConnection& operator=(const DbConnection& other){
          std::cout <<"copy ASSIGNMENT\n";
      delete this->buffer;
      *this = DbConnection(other);
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
