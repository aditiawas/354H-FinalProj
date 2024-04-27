#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<unordered_map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "subdivobjects.h"

using namespace std;

struct loopSubdiv{
    public:

        vector<glm::vec3> vertices;
        vector<TrimeshFace*> trimeshFaces;

    loopSubdiv(vector<glm::vec3> vertices, vector<TrimeshFace*> trimeshfaces)
    {
        this->vertices = vertices;
        this->trimeshFaces = trimeshfaces;

        printf("\nSize of vertices array: %d \n", int(this->vertices.size()));
        printf("\nSize of trimeshfaces array: %d \n", int(this->trimeshFaces.size()));
    }

    glm::vec3 findNewVertex(const glm::vec3& vertex, TrimeshFace* face);
    glm::vec3 findFacePoint(const vector<glm::vec3>& faceVertices);
    void createNewFaces(vector<TrimeshFace*>& newFaces, const vector<glm::vec3>& faceVertices, const vector<glm::vec3>& newVertices);
    void doSubdivision();
    void printSubdividedMesh();

};