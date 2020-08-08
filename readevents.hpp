#include <tuple> 
using namespace std;
#include<stdio.h> 
  

 class Object {      
    public:       
    int id;      
    tuple<float,float> coords; 
    tuple<float,float,float> color;  
    int vertexBufferIndex;
};

enum typ{add, mov, del}; 
 class Event {      
    public:       
    int id;      
    float time;
    enum typ eventType; 
    tuple<float,float> moveCoords;

};