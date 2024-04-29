#include<stdio.h>
#include<stdlib.h>
#include<array>
#ifndef SUBDIVOBJECTS_H_
#define SUBDIVOBJECTS_H_

class TrimeshFace {
public:
    std::array<int, 3> vertexIndices;
    TrimeshFace()
    {
        vertexIndices[0] = 0; 
        vertexIndices[1] = 0;
        vertexIndices[2] = 0;
    }
    TrimeshFace(int vertex1, int vertex2, int vertex3)
    {
        vertexIndices[0] = vertex1;
        vertexIndices[1] = vertex2;
        vertexIndices[2] = vertex3;
    }
};


struct QuadFace{

    int v1,v2,v3,v4; //the vertices corresponding to each face of the quad
    QuadFace()
    {
        v1 = 0; 
        v2 = 0;
        v3 = 0;
        v4 = 0;

    }
    QuadFace(int vertex1, int vertex2, int vertex3, int vertex4)
    {
        v1 = vertex1;
        v2 = vertex2;
        v3 = vertex3;
        v4 = vertex4;

    }
};

struct Quad{
    //not sure what to put here yet
};
#endif