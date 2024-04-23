#include<stdio.h>
#include<stdlib.h>
#include "catmull.h"
using namespace std;

void catmullClark::initializeEdges()
{
    countFacesAdjacent.clear();
    countFacesAdjacent = vector<int>(quadVertices.size(),0); //for each vertex, we set the number of faces its adjacent to to 0
    edgesToFaces.clear();


    for(int i=0;i<quadFaces.size();i++) //we iterate through each face in the quad
    {
        int a_idx = quadFaces[i]->v1;
        int b_idx = quadFaces[i]->v2;
        int c_idx = quadFaces[i]->v3;
        int d_idx = quadFaces[i]->v4;

        //we got indices of the faces 
        //now, for each vertex, it belongs to this face: increase its count
        countFacesAdjacent[a_idx] +=1;
        countFacesAdjacent[b_idx] +=1;
        countFacesAdjacent[c_idx] +=1;
        countFacesAdjacent[d_idx] +=1;

        //for each pair of vertices, form an edge

        string edge1 = to_string(a_idx) + "->" + to_string(b_idx); 



    }

}