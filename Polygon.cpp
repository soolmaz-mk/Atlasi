#include "rapidjson/document.h" // will include "rapidjson/rapidjson.h"
#include <vector> 
#include <tuple> 
using namespace std;

/*struct point{
    float x;
    float y;
};*/

 class Polygon {      
    public:             
    vector<tuple<float,float>> coords;      
};