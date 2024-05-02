#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<fstream>
#include<sstream>
#include "catmull.h"
#include <iostream>
using namespace std;

void catmullClark::initializeEdges()
{
    countFacesAdjacent.clear();
    countSharpEdgesAdjacent.clear();
    verticesToSharpEdgeVertices.clear();
    countFacesAdjacent = vector<int>(quadVertices.size(),0); //for each vertex, we set the number of faces its adjacent to to 0
    countSharpEdgesAdjacent = vector<int>(quadVertices.size(),0);
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

        if(vertex_sharpness[a_idx]==1 && vertex_sharpness[b_idx]==1)
        {
            countSharpEdgesAdjacent[a_idx]+=1;
            countSharpEdgesAdjacent[b_idx]+=1;
            verticesToSharpEdgeVertices[a_idx].push_back(b_idx);
            verticesToSharpEdgeVertices[b_idx].push_back(a_idx);
        }
        if(vertex_sharpness[b_idx]==1 && vertex_sharpness[c_idx]==1)
        {
            countSharpEdgesAdjacent[b_idx]+=1;
            countSharpEdgesAdjacent[c_idx]+=1;
            verticesToSharpEdgeVertices[b_idx].push_back(c_idx);
            verticesToSharpEdgeVertices[c_idx].push_back(b_idx);
        }
        if(vertex_sharpness[c_idx]==1 && vertex_sharpness[d_idx]==1)
        {
            countSharpEdgesAdjacent[c_idx]+=1;
            countSharpEdgesAdjacent[d_idx]+=1;
            verticesToSharpEdgeVertices[c_idx].push_back(d_idx);
            verticesToSharpEdgeVertices[d_idx].push_back(c_idx);
        }
        if(vertex_sharpness[a_idx]==1 && vertex_sharpness[d_idx]==1)
        {
            countSharpEdgesAdjacent[a_idx]+=1;
            countSharpEdgesAdjacent[d_idx]+=1;
            verticesToSharpEdgeVertices[a_idx].push_back(d_idx);
            verticesToSharpEdgeVertices[d_idx].push_back(a_idx);
        }

        //for each pair of vertices, form an edge
        //TOCHECK asmita: do I need to form directed or undirected edges??
        //i dont think so
        //we can use the reverse edges to find the other face 
        string edge1 = to_string(b_idx) + "->" + to_string(a_idx); 
        string edge2 = to_string(c_idx) + "->" + to_string(b_idx);
        string edge3 = to_string(d_idx) + "->" + to_string(c_idx);
        string edge4 = to_string(a_idx) + "->" + to_string(d_idx); //circular

        edgesToFaces[edge1]=i; //mapping each edge to the INDEX of the quadfaces, not the quadfaces themselves to save space
        edgesToFaces[edge2]=i;
        edgesToFaces[edge3]=i;
        edgesToFaces[edge4]=i;
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

void catmullClark::computeEdgePoints(vector<glm::vec3> & facePoints, unordered_map<string,glm::vec3>& edgePoints)
{   

    for(int i=0;i<quadFaces.size();i++)
    {   
        int a_idx = quadFaces[i]->v1;
        int b_idx = quadFaces[i]->v2;
        int c_idx = quadFaces[i]->v3;
        int d_idx = quadFaces[i]->v4;

        //find edges of the quad
        string edge_ba = to_string(b_idx) + "->" + to_string(a_idx); 
        string edge_cb = to_string(c_idx) + "->" + to_string(b_idx);
        string edge_dc = to_string(d_idx) + "->" + to_string(c_idx);
        string edge_ad = to_string(a_idx) + "->" + to_string(d_idx); //circular
        //for each of these edges we have a face point 

        //now, the reverse edges 

        string edge_ab = to_string(a_idx) + "->" + to_string(b_idx); 
        string edge_bc = to_string(b_idx) + "->" + to_string(c_idx);
        string edge_cd = to_string(c_idx) + "->" + to_string(d_idx);
        string edge_da = to_string(d_idx) + "->" + to_string(a_idx); //circular
        //for these edges, we also have a face point
        //we can get this by indexing

        //for edge 1
        int face_adjacent_to_ab_idx = edgesToFaces[edge_ab]; //this wont be our face i (hopefully??)

        glm::vec3 facepoints_ab = facePoints[i] + facePoints[face_adjacent_to_ab_idx];
        glm::vec3 edge_endpoints_ab = quadVertices[a_idx] + quadVertices[b_idx];
        glm::vec3 edgepoints_ab = 0.25f*(facepoints_ab + edge_endpoints_ab);
        if(vertex_sharpness[a_idx]==1 && vertex_sharpness[b_idx]==1) //sharp edge
        {
            edgepoints_ab = 0.5f*(edge_endpoints_ab); //take average of the edge endpoints instead
        }
        //found the edgepoints
        //storing it in both edge_ab and edge_ba for now 

        edgePoints[edge_ab] = edgepoints_ab;
        edgePoints[edge_ba] = edgepoints_ab;

        //for edge 2

        int face_adjacent_to_bc_idx = edgesToFaces[edge_bc]; //this wont be our face i (hopefully??)

        glm::vec3 facepoints_bc = facePoints[i] + facePoints[face_adjacent_to_bc_idx];
        glm::vec3 edge_endpoints_bc = quadVertices[c_idx] + quadVertices[b_idx];
        glm::vec3 edgepoints_bc = 0.25f*(facepoints_bc + edge_endpoints_bc);
        if(vertex_sharpness[c_idx]==1 && vertex_sharpness[b_idx]==1) //sharp edge
        {
            edgepoints_bc = 0.5f*(edge_endpoints_bc); //take average of the edge endpoints instead
        }
        //found the edgepoints
        //storing it in both edge_ab and edge_ba for now 

        edgePoints[edge_bc] = edgepoints_bc;
        edgePoints[edge_cb] = edgepoints_bc;

        //for edge 3

        int face_adjacent_to_cd_idx = edgesToFaces[edge_cd]; //this wont be our face i (hopefully??)

        glm::vec3 facepoints_cd = facePoints[i] + facePoints[face_adjacent_to_cd_idx];
        glm::vec3 edge_endpoints_cd = quadVertices[c_idx] + quadVertices[d_idx];
        glm::vec3 edgepoints_cd = 0.25f*(facepoints_cd + edge_endpoints_cd);
        if(vertex_sharpness[c_idx]==1 && vertex_sharpness[d_idx]==1) //sharp edge
        {
            edgepoints_cd = 0.5f*(edge_endpoints_cd); //take average of the edge endpoints instead
        }
        //found the edgepoints
        //storing it in both edge_ab and edge_ba for now 

        edgePoints[edge_cd] = edgepoints_cd;
        edgePoints[edge_dc] = edgepoints_cd;
        
        //for edge 4

        int face_adjacent_to_da_idx = edgesToFaces[edge_da]; //this wont be our face i (hopefully??)

        glm::vec3 facepoints_da = facePoints[i] + facePoints[face_adjacent_to_da_idx];
        glm::vec3 edge_endpoints_da = quadVertices[a_idx] + quadVertices[d_idx];
        glm::vec3 edgepoints_da = 0.25f*(facepoints_da + edge_endpoints_da);
        if(vertex_sharpness[a_idx]==1 && vertex_sharpness[d_idx]==1) //sharp edge
        {
            edgepoints_da = 0.5f*(edge_endpoints_da); //take average of the edge endpoints instead
        }
        //found the edgepoints
        //storing it in both edge_ab and edge_ba for now 

        edgePoints[edge_da] = edgepoints_da;
        edgePoints[edge_ad] = edgepoints_da;

    }
}

void catmullClark::computeVertexPoints(vector<glm::vec3> & facePoints, unordered_map<string,glm::vec3>& edgePoints, vector<glm::vec3> &vertexPoints)
{   
    vertexPoints.resize(quadVertices.size(), glm::vec3(0.0f));
    for(int i=0;i<quadFaces.size();i++)
    {
        int a_idx = quadFaces[i]->v1;
        int b_idx = quadFaces[i]->v2;
        int c_idx = quadFaces[i]->v3;
        int d_idx = quadFaces[i]->v4;

        float a_divisor = countFacesAdjacent[a_idx];
        float b_divisor = countFacesAdjacent[b_idx];
        float c_divisor = countFacesAdjacent[c_idx];
        float d_divisor = countFacesAdjacent[d_idx];

        if(countSharpEdgesAdjacent[a_idx]<2)
        {
            vertexPoints[a_idx] += (1.0f*facePoints[i])/(a_divisor); //add the contribution of this face to the vertex a
            //F on wikipedia page is the average of all facepoints
            //so while adding each face contribution, divide by total n (where n is the total number of faces touching a)
            vertexPoints[a_idx] += (0.5f*(quadVertices[a_idx] + quadVertices[b_idx]))/(a_divisor); //average of this edge
            vertexPoints[a_idx] += (0.5f*(quadVertices[a_idx] + quadVertices[d_idx]))/(a_divisor); //avergae of this edge 
            //since we have to add 2R to the sum 
            //we can just blindly add each edge to the sum 
            //it will be counted twice for each face 
        }

        if(countSharpEdgesAdjacent[b_idx]<2)
        {
            vertexPoints[b_idx] += (1.0f*facePoints[i])/(b_divisor); //add the contribution of this face to the vertex b
            vertexPoints[b_idx] += (0.5f*(quadVertices[a_idx] + quadVertices[b_idx]))/(b_divisor); 
            vertexPoints[b_idx] += (0.5f*(quadVertices[b_idx] + quadVertices[c_idx]))/(b_divisor); 
        }
        
        if(countSharpEdgesAdjacent[c_idx]<2)
        {
            vertexPoints[c_idx] += (1.0f*facePoints[i])/(c_divisor); //add the contribution of this face to the vertex c
            vertexPoints[c_idx] += (0.5f*(quadVertices[c_idx] + quadVertices[b_idx]))/(c_divisor); 
            vertexPoints[c_idx] += (0.5f*(quadVertices[c_idx] + quadVertices[d_idx]))/(c_divisor); 
        }
        
        if(countSharpEdgesAdjacent[d_idx]<2)
        {
            vertexPoints[d_idx] += (1.0f*facePoints[i])/(d_divisor); //add the contribution of this face to the vertex d
            vertexPoints[d_idx] += (0.5f*(quadVertices[a_idx] + quadVertices[d_idx]))/(d_divisor); 
            vertexPoints[d_idx] += (0.5f*(quadVertices[c_idx] + quadVertices[d_idx]))/(d_divisor); 
        }

        //done adding F
        //done adding 2R
    }
    for(int i =0; i < quadVertices.size(); i++)
    {
        //vertexPoints now has F+2R
        if(vertex_sharpness[i]==1) //sharp vertex: retain!
        {
            vertexPoints[i]=quadVertices[i]; //retain the same position
        }
        else if(countSharpEdgesAdjacent[i]<2) //less than 2 incident edges
        {
            float n = 1.0f*(countFacesAdjacent[i]);
        //TODO asmita possible bug: int to float..
            vertexPoints[i] = (vertexPoints[i] + (n-3)*quadVertices[i])/ (n);
        }
        else if(countSharpEdgesAdjacent[i]==2)
        {
            if(verticesToSharpEdgeVertices[i].size()<2)
            {
                printf("Error: 2 sharp edges but no endpoints!");
            }
            else
            {
                int endpoint1= verticesToSharpEdgeVertices[i][0];
                int endpoint2= verticesToSharpEdgeVertices[i][1];
                vertexPoints[i] = 0.75f*quadVertices[i] + 0.125f*quadVertices[endpoint1] + 0.125f*quadVertices[endpoint2];
            }
        }
        else if(countSharpEdgesAdjacent[i]>2)
        {
            vertexPoints[i]=quadVertices[i]; //retain the same position
        }   
    }

}

std::pair<vector<glm::vec3>, std::vector<QuadFace*> > catmullClark::doSubdivision()
{   
    //opfile = filename + "_op" + std::to_string(iter) + ".out";

    initializeEdges();
    //edges computed, faces touching each vertex counted

    vector<glm::vec3> facePoints;
    computeFacePoints(facePoints);
    //face points computed

    unordered_map<string,glm::vec3> edgePoints;
    computeEdgePoints(facePoints,edgePoints);
    //edge points computed 

    vector<glm::vec3> vertexPoints;
    //vertexPoints.resize(quadVertices.size());
    computeVertexPoints(facePoints,edgePoints,vertexPoints);
    //vertex points computed

    vector<glm::vec3> newQuadVertices = vector<glm::vec3>(0);
    vector<QuadFace*> newQuadFaces = vector <QuadFace*>(0);
    vector<glm::vec3> newQuadNormals = vector<glm::vec3>(0);
    vector<int> newVertexSharpness = vector<int>(0);

    int new_number_of_vertices = 0;
    unordered_map<string, int> edgesToNewVertexIndices; //check if any of our edge points have been already included in the new set of vertices
    unordered_map<int,int> pointsToNewVertexIndices; //check if any of our vertex points have been already included in new set of vertices
    for(int i=0;i<quadFaces.size();i++) //go through each face 
    {

        newQuadVertices.push_back(facePoints[i]); //add this face point to the new vertices 
        newVertexSharpness.push_back(0); //face point is never sharp right?
        int face_point_idx = new_number_of_vertices; //the face point index in the new vertex list is equal to the number of vertices made so far
        new_number_of_vertices +=1;

        //get original vertices
        int a_idx = quadFaces[i]->v1;
        int b_idx = quadFaces[i]->v2;
        int c_idx = quadFaces[i]->v3;
        int d_idx = quadFaces[i]->v4;

        //find edges of the quad
        string edge_ba = to_string(b_idx) + "->" + to_string(a_idx); 
        string edge_cb = to_string(c_idx) + "->" + to_string(b_idx);
        string edge_dc = to_string(d_idx) + "->" + to_string(c_idx);
        string edge_ad = to_string(a_idx) + "->" + to_string(d_idx); 

        //now, the reverse edges 

        string edge_ab = to_string(a_idx) + "->" + to_string(b_idx); 
        string edge_bc = to_string(b_idx) + "->" + to_string(c_idx);
        string edge_cd = to_string(c_idx) + "->" + to_string(d_idx);
        string edge_da = to_string(d_idx) + "->" + to_string(a_idx); 

        int edge_point_for_ab_idx =0; //new vertex index for edge point corresponding to edge ab

        if(edgesToNewVertexIndices.count(edge_ba)) //that edge has been included by another face
        {
            edge_point_for_ab_idx = edgesToNewVertexIndices[edge_ba];
        }
        else if(edgesToNewVertexIndices.count(edge_ab)) //or the reverse edge has been included
        {
            edge_point_for_ab_idx = edgesToNewVertexIndices[edge_ab];
        }
        else
        {
            edge_point_for_ab_idx = new_number_of_vertices; //new vertex to include into list of vertices
            new_number_of_vertices +=1;
            edgesToNewVertexIndices[edge_ba] = edge_point_for_ab_idx;
            newQuadVertices.push_back(edgePoints[edge_ba]); //always has to exist unless something is wrong with my previous logic
            if(vertex_sharpness[a_idx]==1 && vertex_sharpness[b_idx]==1) //sharp edge: the resulting edge point has to be sharp
            {
                newVertexSharpness.push_back(1);
            }
            else
            {
                newVertexSharpness.push_back(0);
            }
        }

        //ab done, now onto bc

        int edge_point_for_bc_idx =0; //new vertex index for edge point corresponding to edge bc

        if(edgesToNewVertexIndices.count(edge_cb)) //that edge has been included by another face
        {
            edge_point_for_bc_idx = edgesToNewVertexIndices[edge_cb];
        }
        else if(edgesToNewVertexIndices.count(edge_bc)) //or the reverse edge has been included
        {
            edge_point_for_bc_idx = edgesToNewVertexIndices[edge_bc];
        }
        else
        {
            edge_point_for_bc_idx = new_number_of_vertices; //new vertex to include into list of vertices
            new_number_of_vertices +=1;
            edgesToNewVertexIndices[edge_cb] = edge_point_for_bc_idx;
            newQuadVertices.push_back(edgePoints[edge_cb]); //always has to exist unless something is wrong with my previous logic
            if(vertex_sharpness[c_idx]==1 && vertex_sharpness[b_idx]==1) //sharp edge: the resulting edge point has to be sharp
            {
                newVertexSharpness.push_back(1);
            }
            else
            {
                newVertexSharpness.push_back(0);
            }
        }
        //bc done

        int edge_point_for_cd_idx =0; //new vertex index for edge point corresponding to edge cd

        if(edgesToNewVertexIndices.count(edge_dc)) //that edge has been included by another face
        {
            edge_point_for_cd_idx = edgesToNewVertexIndices[edge_dc];
        }
        else if(edgesToNewVertexIndices.count(edge_cd)) //or the reverse edge has been included
        {
            edge_point_for_cd_idx = edgesToNewVertexIndices[edge_cd];
        }
        else
        {
            edge_point_for_cd_idx = new_number_of_vertices; //new vertex to include into list of vertices
            new_number_of_vertices +=1;
            edgesToNewVertexIndices[edge_dc] = edge_point_for_cd_idx;
            newQuadVertices.push_back(edgePoints[edge_dc]); //always has to exist unless something is wrong with my previous logic
            if(vertex_sharpness[c_idx]==1 && vertex_sharpness[d_idx]==1) //sharp edge: the resulting edge point has to be sharp
            {
                newVertexSharpness.push_back(1);
            }
            else
            {
                newVertexSharpness.push_back(0);
            }
        }
        //cd done 

        int edge_point_for_da_idx =0; //new vertex index for edge point corresponding to edge cd

        if(edgesToNewVertexIndices.count(edge_ad)) //that edge has been included by another face
        {
            edge_point_for_da_idx = edgesToNewVertexIndices[edge_ad];
        }
        else if(edgesToNewVertexIndices.count(edge_da)) //or the reverse edge has been included
        {
            edge_point_for_da_idx = edgesToNewVertexIndices[edge_da];
        }
        else
        {
            edge_point_for_da_idx = new_number_of_vertices; //new vertex to include into list of vertices
            new_number_of_vertices +=1;
            edgesToNewVertexIndices[edge_ad] = edge_point_for_da_idx;
            newQuadVertices.push_back(edgePoints[edge_ad]); //always has to exist unless something is wrong with my previous logic
            if(vertex_sharpness[c_idx]==1 && vertex_sharpness[d_idx]==1) //sharp edge: the resulting edge point has to be sharp
            {
                newVertexSharpness.push_back(1);
            }
            else
            {
                newVertexSharpness.push_back(0);
            }
        }
        //da done

        //all edge points have been formed presumably 
        //now we do the same thing for vertex points...sigh...

        int vertex_point_for_a_idx=0; //new vertex index for vertex point corresponding to vertex a

        if(pointsToNewVertexIndices.count(a_idx)) //that vertex has been taken before
        {
            vertex_point_for_a_idx = pointsToNewVertexIndices[a_idx];
        }
        else
        {
            vertex_point_for_a_idx = new_number_of_vertices; //new vertex to include into list of vertices
            new_number_of_vertices +=1;
            pointsToNewVertexIndices[a_idx] = vertex_point_for_a_idx;
            newQuadVertices.push_back(vertexPoints[a_idx]); 
            if(vertex_sharpness[a_idx]==1||countSharpEdgesAdjacent[a_idx]>2)
            {
                newVertexSharpness.push_back(1); //sharp vertex
            }
            else
            {
                newVertexSharpness.push_back(0);
            }
        }

        //a done, onto b

        int vertex_point_for_b_idx =0; //new vertex index for vertex point corresponding to vertex b

        if(pointsToNewVertexIndices.count(b_idx)) //that vertex has been taken before
        {
            vertex_point_for_b_idx = pointsToNewVertexIndices[b_idx];
        }
        else
        {
            vertex_point_for_b_idx = new_number_of_vertices; //new vertex to include into list of vertices
            new_number_of_vertices +=1;
            pointsToNewVertexIndices[b_idx] = vertex_point_for_b_idx;
            newQuadVertices.push_back(vertexPoints[b_idx]); 
            if(vertex_sharpness[b_idx]==1||countSharpEdgesAdjacent[b_idx]>2)
            {
                newVertexSharpness.push_back(1); //sharp vertex
            }
            else
            {
                newVertexSharpness.push_back(0);
            }
        }
        //b done

        int vertex_point_for_c_idx =0; //new vertex index for vertex point corresponding to vertex b

        if(pointsToNewVertexIndices.count(c_idx)) //that vertex has been taken before
        {
            vertex_point_for_c_idx = pointsToNewVertexIndices[c_idx];
        }
        else
        {
            vertex_point_for_c_idx = new_number_of_vertices; //new vertex to include into list of vertices
            new_number_of_vertices +=1;
            pointsToNewVertexIndices[c_idx] = vertex_point_for_c_idx;
            newQuadVertices.push_back(vertexPoints[c_idx]); 
            if(vertex_sharpness[c_idx]==1||countSharpEdgesAdjacent[c_idx]>2)
            {
                newVertexSharpness.push_back(1); //sharp vertex
            }
            else
            {
                newVertexSharpness.push_back(0);
            }
        }
        //c done

        int vertex_point_for_d_idx =0; //new vertex index for vertex point corresponding to vertex b

        if(pointsToNewVertexIndices.count(d_idx)) //that vertex has been taken before
        {
            vertex_point_for_d_idx = pointsToNewVertexIndices[d_idx];
        }
        else
        {
            vertex_point_for_d_idx = new_number_of_vertices; //new vertex to include into list of vertices
            new_number_of_vertices +=1;
            pointsToNewVertexIndices[d_idx] = vertex_point_for_d_idx;
            newQuadVertices.push_back(vertexPoints[d_idx]); 
            if(vertex_sharpness[d_idx]==1||countSharpEdgesAdjacent[d_idx]>2)
            {
                newVertexSharpness.push_back(1); //sharp vertex
            }
            else
            {
                newVertexSharpness.push_back(0);
            }
        }

        //we have pushed all the new points
        //now we make the faces corresponding to these points

        /*
        a    ab      b

        ad    f    bc

        d    cd      c
        */
        QuadFace* quad1 = new QuadFace(face_point_idx, edge_point_for_ab_idx, vertex_point_for_a_idx, edge_point_for_da_idx);
        newQuadFaces.push_back(quad1);

        QuadFace* quad2 = new QuadFace(face_point_idx, edge_point_for_bc_idx, vertex_point_for_b_idx, edge_point_for_ab_idx);
        newQuadFaces.push_back(quad2);

        QuadFace* quad3 = new QuadFace(face_point_idx, edge_point_for_cd_idx, vertex_point_for_c_idx, edge_point_for_bc_idx);
        newQuadFaces.push_back(quad3);

        QuadFace* quad4 = new QuadFace(face_point_idx, edge_point_for_da_idx, vertex_point_for_d_idx, edge_point_for_cd_idx);
        newQuadFaces.push_back(quad4);

        //now calculate normals

        /*
        a    ab      b

        ad    f    bc

        d    cd      c
        */

       glm::vec3 fmin1 = newQuadVertices[face_point_idx] - newQuadVertices[edge_point_for_da_idx];
       glm::vec3 fmin2 = newQuadVertices[face_point_idx] - newQuadVertices[edge_point_for_ab_idx];
       fmin1 = glm::normalize(fmin1);
       fmin2 = glm::normalize(fmin2);
       glm::vec3 plane_normal = glm::cross(fmin1,fmin2);
       plane_normal = glm::normalize(plane_normal);
       newQuadNormals.push_back(plane_normal); //for each face, push back the face normal


       fmin1 = newQuadVertices[face_point_idx] - newQuadVertices[edge_point_for_ab_idx];
       fmin2 = newQuadVertices[face_point_idx] - newQuadVertices[edge_point_for_bc_idx];
       fmin1 = glm::normalize(fmin1);
       fmin2 = glm::normalize(fmin2);
       plane_normal = glm::cross(fmin1,fmin2);
       plane_normal = glm::normalize(plane_normal);
       newQuadNormals.push_back(plane_normal); //for each face, push back the face normal

       fmin1 = newQuadVertices[face_point_idx] - newQuadVertices[edge_point_for_bc_idx];
       fmin2 = newQuadVertices[face_point_idx] - newQuadVertices[edge_point_for_cd_idx];
       fmin1 = glm::normalize(fmin1);
       fmin2 = glm::normalize(fmin2);
       plane_normal = glm::cross(fmin1,fmin2);
       plane_normal = glm::normalize(plane_normal);
       newQuadNormals.push_back(plane_normal); //for each face, push back the face normal

       fmin1 = newQuadVertices[face_point_idx] - newQuadVertices[edge_point_for_cd_idx];
       fmin2 = newQuadVertices[face_point_idx] - newQuadVertices[edge_point_for_da_idx];
       fmin1 = glm::normalize(fmin1);
       fmin2 = glm::normalize(fmin2);
       plane_normal = glm::cross(fmin1,fmin2);
       plane_normal = glm::normalize(plane_normal);
       newQuadNormals.push_back(plane_normal); //for each face, push back the face normal

      //normals have been added
    }

    this->quadFaces = newQuadFaces;
    this->quadNormals = newQuadNormals;
    this->quadVertices = newQuadVertices;
    this->vertex_sharpness=newVertexSharpness;

    countFacesAdjacent.clear();
    countSharpEdgesAdjacent.clear();
    edgesToFaces.clear();
    verticesToSharpEdgeVertices.clear();

    //printf("\n After subdivision \n");
    
    //printf("\n After subdivision Size of quadVertices %d \n", int(quadVertices.size()));
    //printf("\n After subdivision Size of quadFaces %d \n", int(quadFaces.size()));
    //printf("\n After subdivision Size of quadNormals %d \n", int(quadNormals.size()));
    return std::make_pair(newQuadVertices, newQuadFaces);

    // displayScene();

}

void catmullClark::printSubdividedMesh(vector<glm::vec3> v, vector<QuadFace*> f) {
    // Open the output file
    //printf("Output file: %s", opfile.c_str());

    const std::string output_file = opfile;
    std::ofstream outputFile(output_file);
    if (!outputFile.is_open()) {
        printf("Failed to open output file \n");
        return;
    }

    // Print vertices to console and file
    for (const glm::vec3& vertex : v) {
        //std::cout << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
        outputFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }

    // Print trimesh faces to console and file
    for (const QuadFace* face : f) {
        //std::cout << "f " << face.vertexIndices[0] + 1 << " " << face.vertexIndices[1] + 1 << " " << face.vertexIndices[2] + 1 << "\n";
        outputFile << "f " << face->v1 + 1 << " " << face->v2 + 1 << " " << face->v3 + 1 << " " << face->v4 + 1 << "\n";
    }

    // Close the output file
    outputFile.close();
}

void catmullClark::displayScene() {
    // Path to the executable
    std::string executable = "./render";

    // Arguments to pass to the executable
    std::string arg1 = opfile;

    // Construct the command string with arguments
    std::string command = executable + " " + arg1 + " 0";

    int result = system(command.c_str());

    // Check the return value
    if (result != 0)
        std::cout << "\nFailed to run the executable." << std::endl;
}

string catmullClark::doSubdivisionIteratively(int iter, std::string filename)
{

    opfile = filename;
    std::stringstream ss;

    if (iter > 0)
    {
        opfile = filename + "_op" + std::to_string(iter) + ".out";
        printf("\nPerforming CatmullClark Subdivision\n");
        ss << "\nOriginal Mesh Info:\n" << quadVertices.size() << " vertices, " << quadFaces.size() << " faces\n";
        vector<glm::vec3> oldQuadVertices = this->quadVertices;
        vector<QuadFace*> oldQuadFaces = this->quadFaces;
        vector<glm::vec3> oldQuadNormals = this->quadNormals;
        vector<int> oldVertexSharpness = this->vertex_sharpness;
        for(int i=0; i<iter;i++)
        {
            doSubdivision();
        }
        ss << "\nSubdivided Mesh Info:\n" << quadVertices.size() << " vertices, " << quadFaces.size() << " faces\n";

        //printf("\nAfter subdivision \n");
        printSubdividedMesh(quadVertices, quadFaces);
        //restoring the original mesh
        this->quadVertices = oldQuadVertices;
        this->quadFaces = oldQuadFaces;
        this->quadNormals = oldQuadNormals;
        this->vertex_sharpness=oldVertexSharpness;
    }
    string log = ss.str();
    displayScene();
    return log;
}
