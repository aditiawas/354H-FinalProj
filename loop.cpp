#include <stdio.h>
#include <stdlib.h>
#include "loop.h"
#include <string>
#include <unordered_set>
#include <numeric>
#include <iostream>
#include <fstream>
using namespace std;

//custom hash function
struct hash_vec3 {
    size_t operator()(const glm::vec3& v) const {
        return hash<float>()(v.x) ^ hash<float>()(v.y) ^ hash<float>()(v.z);
    }
};

void loopSubdiv::printSubdividedMesh() {

    printf("\nSize of Vertices array: %d \n", int(vertices.size()));
    printf("\nSize of trimeshfaces array: %d \n", int(trimeshFaces.size()));

    for (const glm::vec3& vertex : vertices)
        std::cout << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";

    for (TrimeshFace* face : trimeshFaces)
        std::cout << "f " << face->v1 + 1 << " " << face->v2 + 1 << " " << face->v3 + 1 << "\n";
}

void loopSubdiv::doSubdivision() {

    printf("\nPerforming Loop Subdivision\n");

    //set to store unique vertices
    unordered_set<glm::vec3, hash_vec3> uniqueVertices(vertices.begin(), vertices.end());

    vector<glm::vec3> newVertices;
    vector<TrimeshFace*> newFaces;

    for (TrimeshFace* face : trimeshFaces) {
        unsigned int faceVertexIndices[3] = { face->v1, face->v2, face->v3 };

        vector<glm::vec3> faceVertices;
        for (int i = 0; i < 3; i++) {
            glm::vec3 newVertex = findNewVertex(vertices[faceVertexIndices[i]], face);
            uniqueVertices.insert(newVertex);
            newVertices.push_back(newVertex);
            faceVertices.push_back(newVertex);
        }

        glm::vec3 facePoint = findFacePoint(faceVertices);
        uniqueVertices.insert(facePoint);
        newVertices.push_back(facePoint);

        createNewFaces(newFaces, faceVertices, newVertices);
    }

    //updating in object
    vertices = vector<glm::vec3>(uniqueVertices.begin(), uniqueVertices.end());
    trimeshFaces = newFaces;

    printf("\nAfter subdivision \n");
    printSubdividedMesh();
}

glm::vec3 loopSubdiv::findNewVertex(const glm::vec3& vertex, TrimeshFace* face) {
    unsigned int faceVertices[3] = { face->v1, face->v2, face->v3 };
    int n = 3;

    //find neighborhood of vertex
    vector<glm::vec3> neighborhood;
    for (int i = 0; i < n; i++) {
        if (vertices[faceVertices[i]] == vertex) {
            neighborhood.push_back(vertices[faceVertices[(i + 1) % n]]);
            neighborhood.push_back(vertices[faceVertices[(i + n - 1) % n]]);
        }
    }

    //apply Loop's rules for vertex calculation
    glm::vec3 newVertex = (1.0f / n) * vertex;
    for (const glm::vec3& neighborVertex : neighborhood) {
        newVertex += (3.0f / (8.0f * n)) * neighborVertex;
    }

    return newVertex;
}

glm::vec3 loopSubdiv::findFacePoint(const vector<glm::vec3>& faceVertices) {
    int n = faceVertices.size();
    glm::vec3 facePoint(0.0f);

    //apply Loop's rules for face point calculation
    for (const glm::vec3& vertex : faceVertices) {
        facePoint += vertex;
    }
    facePoint /= n;

    return facePoint;
}

void loopSubdiv::createNewFaces(vector<TrimeshFace*>& newFaces, const vector<glm::vec3>& faceVertices, const vector<glm::vec3>& newVertices) {
    int n = faceVertices.size();
    int vertexIndex = newVertices.size() - faceVertices.size() - 1;

    //create new faces
    for (int i = 0; i < n; i++) {
        vector<unsigned int> faceIndices;
        for (int j = 0; j < n; j++) {
            faceIndices.push_back(static_cast<unsigned int>(vertices.size() + vertexIndex + j));
        }
        faceIndices.push_back(static_cast<unsigned int>(vertices.size() + newVertices.size() - 1));
        newFaces.push_back(new TrimeshFace(faceIndices[0], faceIndices[1], faceIndices[2]));
    }
}