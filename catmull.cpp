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
        //TOCHECK asmita: do I need to form directed or undirected edges??
        string edge1 = to_string(b_idx) + "->" + to_string(a_idx); 
        string edge2 = to_string(c_idx) + "->" + to_string(b_idx);
        string edge3 = to_string(d_idx) + "->" + to_string(c_idx);
        string edge4 = to_string(a_idx) + "->" + to_string(d_idx); //circular

        edgesToFaces[edge1] = i; //mapping each edge to the INDEX of the quadfaces, not the quadfaces themselves to save space
        edgesToFaces[edge2] = i;
        edgesToFaces[edge3] = i;
        edgesToFaces[edge4] = i;
    }

}
void catmullClark::computeFacePoints(vector<glm::vec3> & facePoints)
{   
    glm::vec3 facept;
    for(int i=0;i<quadFaces.size();i++)
    {
        int a_idx = quadFaces[i]->v1;
        int b_idx = quadFaces[i]->v2;
        int c_idx = quadFaces[i]->v3;
        int d_idx = quadFaces[i]->v4;
        facept = quadVertices[a_idx] + quadVertices[b_idx] + quadVertices[c_idx] + quadVertices[d_idx];
        facept = 0.25f*facept;
        facePoints.push_back(facept);
    }
    return;
}
void catmullClark::doSubdivision()
{

}