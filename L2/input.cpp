#include <iostream>
#include <cmath>
using namespace std;


struct Point{
   int x,y:w;
};

double getDistance(Point a,Point b){
  double manhatan = (a.x - b.x)*(a.x-b.x) + (a.y -b.y)*(a.y -b.y);
  double euclidean = sqrt(manhatan);
  return euclidean;  
}
int main(){
  Point a {0,0};
  Point b {3,4};
  cout << getDistance(a,b) << endl;	
}
