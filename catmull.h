#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "subdivobjects.h"
using namespace std;

struct catmullClark{

    vector<glm::vec3> quadVertices;
    vector<QuadFace*> quadFaces;
    vector<glm::vec3> quadNormals;

    catmullClark(vector<glm::vec3> vertices, vector<QuadFace*> faces )
    {
        quadVertices = vertices;
        quadFaces = faces;
        quadNormals.clear();

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

        printf("Size of quadVertices %d", int(quadVertices.size()));
        printf("Size of quadFaces %d", int(quadFaces.size()));
        printf("Size of quadNormals %d", int(quadNormals.size()));

    }

};