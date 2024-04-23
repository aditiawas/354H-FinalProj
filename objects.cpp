#include<stdio.h>
#include<stdlib.h>
using namespace std;

struct TrimeshFace{

    int v1,v2,v3; //the vertices corresponding to each face of the trimesh
    TrimeshFace()
    {
        v1 = 0; 
        v2 = 0;
        v3 = 0;

    }
    TrimeshFace(int vertex1, int vertex2, int vertex3)
    {
        v1 = vertex1;
        v2 = vertex2;
        v3 = vertex3;

    }

};

struct Trimesh{
    //not sure what to put here yet
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

