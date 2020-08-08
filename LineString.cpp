#include "rapidjson/document.h" // will include "rapidjson/rapidjson.h"
#include <vector> 
#include <tuple> 
using namespace std; 

/*struct point{
    float x;
    float y;
};*/

 class LineString {      
    public:             
    vector<tuple<float,float>> coords;     
  //  vector<float arr[2]> coords;
};