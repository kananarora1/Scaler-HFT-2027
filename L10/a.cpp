

#include <iostream>
using namespace std;


template<typename T>
struct Rank {
  constexpr static int res = 0;
};

template<typename T>
struct Rank<T*> {
	constexpr static int res = 1 + Rank<T>::res;
};

int main(){
    cout << Rank<int>::res << endl;
}

template<int N>
struct LogCalculator{
    static int constexpr result = LogCalculator<N/2>::result + 1; 
};

template<>
struct LogCalculator<1>{
  static int constexpr result = 0;  
};

int logBase2(int val){
    if(val <= 1)
        return 0;
    return logBase2(val/2) + 1;
}

template<typename T>
class Vector {
public:
    Vector(int size){
        this->mSize = size;
        this->mData = new T[mSize];
    }
    
    Vector(const Vector& other){
        this->mSize = other.mSize;
        this->mData = new T[mSize];
        for(int i=0; i<mSize; i++){
            mData[i]=other.mData[i];
        }
    }
    Vector(Vector&& other){
        mData=other.mData;
        mSize=other.mSize;
        other.mSize = 0;
        other.mData = nullptr;
    }
    
    Vector& operator=(const Vector& other){
        if(this != &other){
            this.mSize = other.mSize;
            delete [] this.mData;
            this.mData = new T[mSize];
            
            for(int i =0; i<mSize; i++){
                mData[i] = other.mData[i];
            }
        }
        return *this;
    } // A = B ... operator-(B)... thos = A
    Vector& operator-=(Vector&& other){
        delete [] this->mData;
        this->mData = other.mData;
        this->mSize = other.mSize;
        other.mData = nullptr;
        other.mSize = 0;
    }
    
    ~Vector(){
        delete [] mData;
    }
private:
    int mSize;
    T* mData;
};

class Car{
public:
    Car(){}
    Car(const Car& other){
        this->mWheels = other.mWheels; // TODO FOR US 
        this->mEngine = other.engine;
        this-<mId = 36328767;
        this->mNumberPlate = "dfgjhdfsgdsj";
    }
    Car(Car&& other){
        this->mNumberPlate = std::move(other.mNumberPlate); // TODO 
        this->mId = mId;
        this->mWheels = std::move(other.mWheels);
        this->mEngin = std::move(other.mEngine);
    }
    ~Car(){}
private:
    std::vector<Wheels> mWheels;
    Engine mEngine;
    int mId;
    std::string mNumberPlate;
};
