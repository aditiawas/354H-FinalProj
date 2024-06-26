#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<unordered_map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "subdivobjects.h"
using namespace std;

struct catmullClark{
    public:
        std::string opfile;
        vector<glm::vec3> quadVertices;
        vector<QuadFace*> quadFaces;
        vector<glm::vec3> quadNormals;
        vector<int> vertex_sharpness;
        vector<int> countFacesAdjacent;
        vector<int> countSharpEdgesAdjacent;
        unordered_map<string,int> edgesToFaces; //edge mapped to each face it is a part of
        unordered_map<int,vector<int> > verticesToSharpEdgeVertices; //vertex mapped to the other endpoints of the sharp edges that are incident upon it
        void initializeEdges();
        void computeFacePoints(vector<glm::vec3>&);
        void computeEdgePoints(vector<glm::vec3>&, unordered_map<string,glm::vec3>&);
        void computeVertexPoints(vector<glm::vec3>&, unordered_map<string,glm::vec3>&, vector<glm::vec3>&);
        std::pair<vector<glm::vec3>, std::vector<QuadFace*> > doSubdivision();
        string doSubdivisionIteratively(int, std::string);
        void printSubdividedMesh(vector<glm::vec3>, vector<QuadFace*>);
        void displayScene();


    catmullClark(vector<glm::vec3> vertices, vector<QuadFace*> faces )
    {
        quadVertices = vertices;
        quadFaces = faces;
        vertex_sharpness = vector<int>(vertices.size(),0);
        quadNormals.clear();
        countFacesAdjacent.clear();
        edgesToFaces.clear();

        for(int i=0; i<quadFaces.size();i++)
        {
            int a_idx = quadFaces[i]->v1; //remember, we stored indexes of the vertices in the quad array
            int b_idx = quadFaces[i]->v2;
            int c_idx = quadFaces[i]->v3;

            glm::vec3 bmina = quadVertices[b_idx] - quadVertices[a_idx];
            glm::vec3 cmina = quadVertices[c_idx] - quadVertices[a_idx];
            bmina = glm::normalize(bmina);
            cmina = glm::normalize(cmina);

            glm::vec3 plane_normal = glm::cross(bmina,cmina);
            plane_normal = glm::normalize(plane_normal);

            quadNormals.push_back(plane_normal); //for each face, push back the face normal

        }

        printf("\n Size of quadVertices %d \n", int(quadVertices.size()));
        printf("\n Size of quadFaces %d \n", int(quadFaces.size()));
        printf("\n Size of quadNormals %d \n", int(quadNormals.size()));

    }

    catmullClark(vector<glm::vec3> vertices, vector<QuadFace*> faces, vector<int> sharpness )
    {
        quadVertices = vertices;
        quadFaces = faces;
        vertex_sharpness = sharpness;
        quadNormals.clear();
        countFacesAdjacent.clear();
        edgesToFaces.clear();

        for(int i=0; i<quadFaces.size();i++)
        {
            int a_idx = quadFaces[i]->v1; //remember, we stored indexes of the vertices in the quad array
            int b_idx = quadFaces[i]->v2;
            int c_idx = quadFaces[i]->v3;

            glm::vec3 bmina = quadVertices[b_idx] - quadVertices[a_idx];
            glm::vec3 cmina = quadVertices[c_idx] - quadVertices[a_idx];
            bmina = glm::normalize(bmina);
            cmina = glm::normalize(cmina);

            glm::vec3 plane_normal = glm::cross(bmina,cmina);
            plane_normal = glm::normalize(plane_normal);

            quadNormals.push_back(plane_normal); //for each face, push back the face normal

        }

        printf("\n Size of quadVertices %d \n", int(quadVertices.size()));
        printf("\n Size of quadFaces %d \n", int(quadFaces.size()));
        printf("\n Size of quadNormals %d \n", int(quadNormals.size()));

    }

};
