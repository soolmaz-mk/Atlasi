#include "rapidjson/document.h" // will include "rapidjson/rapidjson.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <iostream>
#include <typeinfo>
#include <vector> 
#include <map>
#include "readevents.hpp"
#include<algorithm>
#include <string.h>
#include <sstream>
#include <tuple>
using namespace std;
using namespace rapidjson;
using namespace std;


/*tuple<float,float> transform(float x, float y) {
    float MINx = 51.377342;
    float MAXx = 51.410731;
    float MINy = 35.689496;
    float MAXy = 35.714031;


    float distx = MAXx-MINx;
    float disty = MAXy-MINy;

    float dx = x-MINx;
    float dy = y-MINy;
    float xratio = (dx/distx)*2;
    float yratio = (dy/disty)*2;
    float xres = -1+xratio;
    float yres = -1+yratio;

    tuple<float,float> t (xres,yres);
    return t;
}*/


tuple<float, float, float> convertColor(string color){
    string r = color.substr(1, 2);
    string g = color.substr(3, 2);
    string b = color.substr(5, 2);
    signed int rInt;
    int gInt;
    int bInt;
    stringstream sr(r);
    sr >> std::hex >> rInt;
    stringstream sg(g);
    sg >> std::hex >> gInt;
    stringstream sb(b);
    sb >> std::hex >> bInt;
    tuple<float,float,float> tu;
    get<0>(tu) = rInt/255.0;
    get<1>(tu) = gInt/255.0;
    get<2>(tu) = bInt/255.0;
    return tu;
}


tuple<vector<Event>,map<int, Object>> readEvents(){
    FILE* fp = fopen("events.json", "rb"); // non-Windows use "r"
    
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    
    Document d;
    d.ParseStream(is); 
    fclose(fp);
    
    int eventSize=d.Size();

    vector<Event> events;
    map<int, Object> objects;
    
    for (int i = 0; i <eventSize ; i++){
            
            const Value& typ = d[i]["type"];
            if(strcmp(typ.GetString(), "add") == 0){
                const Value& coord = d[i]["object"]["coords"];
                const Value& id = d[i]["object"]["id"];
                const Value& color = d[i]["object"]["color"];
                const Value& time = d[i]["timestamp"];

                float x = coord[0].GetFloat();
                float y = coord[1].GetFloat();
                tuple<float,float> temp = transform(x,y);
                
                Object ob;
                ob.id = id.GetInt();
                get<0>(ob.coords) = get<0>(temp);
                get<1>(ob.coords) = get<1>(temp);
                ob.color = convertColor(color.GetString());
                ob.vertexBufferIndex = NULL;
                //objects[id.GetInt()] = ob;
                 objects.insert(pair<int, Object>(id.GetInt(), ob));

               // enum typ {add, move, del}; 

                Event ev;
                ev.id = id.GetInt();
                ev.time = time.GetFloat()/3750;
                ev.eventType = add;
                get<0>(ev.moveCoords) = NULL;
                get<1>(ev.moveCoords) = NULL;
                events.push_back(ev);
            }
            

            if(strcmp(typ.GetString(), "move") == 0){
                const Value& coord = d[i]["coords"];
                const Value& id = d[i]["object_id"];
                const Value& time = d[i]["timestamp"];

                float x = coord[0].GetFloat();
                float y = coord[1].GetFloat();
                tuple<float,float> temp = transform(x,y);

                Event ev;
                ev.id = id.GetInt();
                ev.time = time.GetFloat()/3750;
                ev.eventType = mov;
                get<0>(ev.moveCoords) = get<0>(temp);
                get<1>(ev.moveCoords) = get<1>(temp);
                events.push_back(ev);

            }
    
            if(strcmp(typ.GetString(), "delete") == 0){
                const Value& id = d[i]["object_id"];
                const Value& time = d[i]["timestamp"];

                Event ev;
                ev.id = id.GetInt();
                ev.time = time.GetFloat()/3750;
                ev.eventType = del;
                events.push_back(ev);

            }
    }
    //sort events by time
   sort(events.begin(), events.end(), [](const Event& rhs, const Event& lhs) {
      return lhs.time < rhs.time;
   });
   /*for(int i=0; i<events.size()-1; i++){
       cout<<i<<" ";
       cout<<events.at(i).time<<"\n";
    //if(events.at(i).time==events.at(i+1).time)
      //  cout<<i<<"\n";
   }*/
   for(int i=0; i<185; i++){
       cout<<get<0>(objects[i].coords)<<" ";
       cout<<get<1>(objects[i].coords)<<"\n";
   }

    
    tuple<vector<Event>,map<int, Object>> t;
    get<0>(t) = events;
    get<1>(t) = objects;
    return t;
}