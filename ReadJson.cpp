#include "rapidjson/document.h" // will include "rapidjson/rapidjson.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <iostream>
#include <typeinfo>
#include"Polygon.cpp"
#include "LineString.cpp"
#include <vector> 
using namespace std;
using namespace rapidjson;

// ...

tuple<float,float> transform(float x, float y) {
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

   /* cout<<distx<<"\n";
    cout<<disty<<"\n";
    cout<<dx<<"\n";
    cout<<dy<<"\n";
    cout<<xres<<"\n";
    cout<<yres<<"\n";*/
   /* tuple<float,float> t ;
    get<0>(t) = xres; 
    get<1>(t) = yres; */
    tuple<float,float> t (xres,yres);
    //t = make_tuple(xres, yres); 
    return t;
}


tuple<vector<LineString>,vector<Polygon>> read() {

    FILE* fp = fopen("geoData.json", "rb"); // non-Windows use "r"
    
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    
    Document d;
    d.ParseStream(is); 
    fclose(fp);

    int FeatureSize=d["features"].Size();
    vector<Polygon> ply;
    vector<LineString> line;
    vector<tuple<float,float>> crd;
    //printf("1\n");
    int f = FeatureSize-1;
    int IsLine;
    for (int i = 0; i <FeatureSize ; i++){
            
            const Value& typ = d["features"][i]["geometry"]["type"];
            const Value& coord = d["features"][i]["geometry"]["coordinates"];
            int CoordSize=coord.Size();
            //cout<<CoordSize<<"\n";
            crd.clear();
            IsLine=0;
        if(strcmp(typ.GetString(), "LineString") == 0){
            IsLine=1;
            for (int j = 0; j <CoordSize ; j++){
                float x = coord[j][0].GetFloat();
                float y = coord[j][1].GetFloat();
                std::tuple<float,float> t (x,y);
                crd.push_back(t); 
            }
        }
            else{
                int CrdSize=coord[0].Size();
               // cout<<CrdSize<<"\n";
                for (int j = 0; j <CrdSize ; j++){
                float x = coord[0][j][0].GetFloat();
                float y = coord[0][j][1].GetFloat();
                std::tuple<float,float> t (x,y);
                crd.push_back(t); 
            }
                //continue;
                /*if(coord.Size()!=1){
                cout<<i<<" ";
                cout<<coord.Size()<<"\n";
                }
               // assert(coord.Size()==1);
               // cout<<i<<"\n";
                float x = coord[0][j][0].GetFloat();
                float y = coord[0][j][1].GetFloat();
                std::tuple<float,float> t (x,y);
                crd.push_back(t); 
                Polygon p;
                p.coords=crd;
                ply.push_back(p);*/
            }
            //printf("a[%d] = %f\n", i, coord[j][0].GetFloat());
            //printf("a[%d] = %f\n", i, coord[j][1].GetFloat());
        if(IsLine==1){
            LineString l;
            l.coords=crd;
            line.push_back(l);
        }
        else{
            
            Polygon p;
            p.coords=crd;
            ply.push_back(p);
        }
    }
  //  LineString & ll = line.at(0);
    //tuple<float,float> t=ll.coords.at(0);

    int LineSize = line.size();

    for (int i=0; i<LineSize; i++){
        LineString & l = line.at(i);
        int CoordSize = l.coords.size();
        for(int j=0; j<CoordSize; j++){
            tuple<float,float> t = l.coords.at(j);
            //cout<<get<0>(t)<<" ";
            //cout<<get<1>(t)<<"\n";
            float x = get<0>(t);
            float y = get<1>(t);
            //cout<<x<<" ";
            //cout<<y<<"\n";
            tuple<float,float> temp = transform(x,y);
            get<0>(l.coords.at(j)) = get<0>(temp);
            get<1>(l.coords.at(j)) = get<1>(temp);
            //cout<<get<0>(temp)<<" ";
            //cout<<get<1>(temp)<<"\n";
            //tuple<float,float> tt=l.coords.at(j);
            //cout<<get<0>(tt)<<" ";
            //cout<<get<1>(tt)<<"\n";
            //cout<<j<<"\n";
        }
        //cout<<CoordSize<<"\n";
    }

    int PolySize = ply.size();
    cout<<PolySize<<"\n";
    int flag=0;
    for (int i=0; i<PolySize; i++){
        Polygon & p = ply.at(i);
        int CoordSize = p.coords.size();
        flag=0;
        for(int j=0; j<CoordSize; j++){
            tuple<float,float> t = p.coords.at(j);

            float x = get<0>(t);
            float y = get<1>(t);
            //cout<<x<<" ";
            //cout<<y<<"\n";

            tuple<float,float> temp = transform(x,y);
            get<0>(p.coords.at(j)) = get<0>(temp);
            get<1>(p.coords.at(j)) = get<1>(temp);
            //cout<<get<0>(temp)<<" ";
            //cout<<get<1>(temp)<<"\n";
            if(get<0>(temp)<1 && get<0>(temp)>-1)
                flag=1;

        }
        //if(flag==1)
          //  cout<<i<<"\n";
    }
    tuple<vector<LineString>,vector<Polygon>> t (line,ply);

    return t;
}



