// loop.cpp
#include <stdio.h>
#include <stdlib.h>
#include "loop.h"
#include <string>
#include <unordered_set>
#include <numeric>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <array>
#include <sstream>

using namespace std;

void LoopSubdiv::printSubdividedMesh(std::vector<glm::vec3> v, std::vector<TrimeshFace> f) {
    // Open the output file
    //printf("Output file: %s", opfile.c_str());
    std::ofstream outFile(opfile);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file" << std::endl;
        return;
    }

    // Print vertices to console and file
    for (const glm::vec3& vertex : v) {
        //std::cout << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
        outFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }

    // Print trimesh faces to console and file
    for (const TrimeshFace face : f) {
        //std::cout << "f " << face.vertexIndices[0] + 1 << " " << face.vertexIndices[1] + 1 << " " << face.vertexIndices[2] + 1 << "\n";
        outFile << "f " << face.vertexIndices[0] + 1 << " " << face.vertexIndices[1] + 1 << " " << face.vertexIndices[2] + 1 << "\n";
    }

    // Close the output file
    outFile.close();
}

bool operator==(const glm::vec3& x, const glm::vec3& y) {
    return x.x == y.x && x.y == y.y && x.z == y.z;
}

double sumXCoords(double sum, const glm::vec3& v) {
    return sum + v.x;
}

double sumYCoords(double sum, const glm::vec3& v) {
    return sum + v.y;
}

double sumZCoords(double sum, const glm::vec3& v) {
    return sum + v.z;
}

std::pair<vector<glm::vec3>, std::vector<TrimeshFace> > LoopSubdiv::subdivLoop(const vector<glm::vec3>& vertices, const std::vector<TrimeshFace>& faces, int iterations) {
    vector<glm::vec3> currentVertices = vertices;
    std::vector<TrimeshFace> currentFaces = faces;

    for (int i = 0; i < iterations; ++i) {
        vector<glm::vec3> newVertices;
        std::vector<TrimeshFace> newFaces;

        std::unordered_map<std::string, HalfEdge> halfEdgeDS;

        // Build half-edge data structure
        for (const TrimeshFace& face : currentFaces) {
            for (int j = 0; j < 3; ++j) {
                std::string name = std::to_string(face.vertexIndices[j]) + std::to_string(face.vertexIndices[(j + 1) % 3]);
                std::string revName = std::to_string(face.vertexIndices[(j + 1) % 3]) + std::to_string(face.vertexIndices[j]);
                HalfEdge he;
                he.vertices[0] = currentVertices[face.vertexIndices[j]];
                he.vertices[1] = currentVertices[face.vertexIndices[(j + 1) % 3]];
                he.pairHalfEdgeKey = revName;
                he.face = face.vertexIndices;
                he.nextHalfEdgeVertices[0] = currentVertices[face.vertexIndices[(j + 1) % 3]];
                he.nextHalfEdgeVertices[1] = currentVertices[face.vertexIndices[(j - 1 + 3) % 3]];
                halfEdgeDS[name] = he;
            }
        }

        std::unordered_map<std::string, int> vertexLocToIdx;
        std::unordered_map<int, vector<glm::vec3> > oldVertexNeighbors;

        for (const TrimeshFace& face : currentFaces) {
            HalfEdge anyHeInFace = halfEdgeDS[std::to_string(face.vertexIndices[0]) + std::to_string(face.vertexIndices[1])];
            HalfEdge nextHe = halfEdgeDS[std::to_string(face.vertexIndices[1]) + std::to_string(face.vertexIndices[2])];
            HalfEdge nextToNextHe = halfEdgeDS[std::to_string(face.vertexIndices[2]) + std::to_string(face.vertexIndices[0])];

            // Update new coordinates inline
            glm::vec3 midPointA(
                (3.0 / 8.0) * anyHeInFace.vertices[0].x + (3.0 / 8.0) * anyHeInFace.vertices[1].x +
                (1.0 / 8.0) * nextHe.vertices[1].x + (1.0 / 8.0) * halfEdgeDS[anyHeInFace.pairHalfEdgeKey].nextHalfEdgeVertices[1].x,
                (3.0 / 8.0) * anyHeInFace.vertices[0].y + (3.0 / 8.0) * anyHeInFace.vertices[1].y +
                (1.0 / 8.0) * nextHe.vertices[1].y + (1.0 / 8.0) * halfEdgeDS[anyHeInFace.pairHalfEdgeKey].nextHalfEdgeVertices[1].y,
                (3.0 / 8.0) * anyHeInFace.vertices[0].z + (3.0 / 8.0) * anyHeInFace.vertices[1].z +
                (1.0 / 8.0) * nextHe.vertices[1].z + (1.0 / 8.0) * halfEdgeDS[anyHeInFace.pairHalfEdgeKey].nextHalfEdgeVertices[1].z
            );

            glm::vec3 midPointC(
                (3.0 / 8.0) * nextHe.vertices[0].x + (3.0 / 8.0) * nextHe.vertices[1].x +
                (1.0 / 8.0) * nextToNextHe.vertices[1].x + (1.0 / 8.0) * halfEdgeDS[nextHe.pairHalfEdgeKey].nextHalfEdgeVertices[1].x,
                (3.0 / 8.0) * nextHe.vertices[0].y + (3.0 / 8.0) * nextHe.vertices[1].y +
                (1.0 / 8.0) * nextToNextHe.vertices[1].y + (1.0 / 8.0) * halfEdgeDS[nextHe.pairHalfEdgeKey].nextHalfEdgeVertices[1].y,
                (3.0 / 8.0) * nextHe.vertices[0].z + (3.0 / 8.0) * nextHe.vertices[1].z +
                (1.0 / 8.0) * nextToNextHe.vertices[1].z + (1.0 / 8.0) * halfEdgeDS[nextHe.pairHalfEdgeKey].nextHalfEdgeVertices[1].z
            );

            glm::vec3 midPointE(
                (3.0 / 8.0) * nextToNextHe.vertices[0].x + (3.0 / 8.0) * nextToNextHe.vertices[1].x +
                (1.0 / 8.0) * anyHeInFace.vertices[1].x + (1.0 / 8.0) * halfEdgeDS[nextToNextHe.pairHalfEdgeKey].nextHalfEdgeVertices[1].x,
                (3.0 / 8.0) * nextToNextHe.vertices[0].y + (3.0 / 8.0) * nextToNextHe.vertices[1].y +
                (1.0 / 8.0) * anyHeInFace.vertices[1].y + (1.0 / 8.0) * halfEdgeDS[nextToNextHe.pairHalfEdgeKey].nextHalfEdgeVertices[1].y,
                (3.0 / 8.0) * nextToNextHe.vertices[0].z + (3.0 / 8.0) * nextToNextHe.vertices[1].z +
                (1.0 / 8.0) * anyHeInFace.vertices[1].z + (1.0 / 8.0) * halfEdgeDS[nextToNextHe.pairHalfEdgeKey].nextHalfEdgeVertices[1].z
            );

            int miniAIdx;
            std::string aKey = std::to_string(face.vertexIndices[0]) + "-" + std::to_string(face.vertexIndices[1]);
            std::string aRevKey = std::to_string(face.vertexIndices[1]) + "-" + std::to_string(face.vertexIndices[0]);
            if (vertexLocToIdx.count(aKey)) {
                miniAIdx = vertexLocToIdx[aKey];
            } else if (vertexLocToIdx.count(aRevKey)) {
                miniAIdx = vertexLocToIdx[aRevKey];
            } else {
                newVertices.push_back(midPointA);
                miniAIdx = newVertices.size() - 1;
                vertexLocToIdx[aKey] = miniAIdx;
            }

            int miniBIdx;
            std::string bKey = std::to_string(face.vertexIndices[1]);
            if (vertexLocToIdx.count(bKey)) {
                miniBIdx = vertexLocToIdx[bKey];
            } else {
                newVertices.push_back(anyHeInFace.vertices[1]);
                miniBIdx = newVertices.size() - 1;
                vertexLocToIdx[bKey] = miniBIdx;
            }

            if (oldVertexNeighbors.count(miniBIdx)) {
                oldVertexNeighbors[miniBIdx].push_back(anyHeInFace.vertices[0]);
            } else {
                oldVertexNeighbors[miniBIdx] = std::vector<glm::vec3>(1, anyHeInFace.vertices[0]);
            }

            int miniCIdx;
            std::string cKey = std::to_string(face.vertexIndices[1]) + "-" + std::to_string(face.vertexIndices[2]);
            std::string cRevKey = std::to_string(face.vertexIndices[2]) + "-" + std::to_string(face.vertexIndices[1]);
            if (vertexLocToIdx.count(cKey)) {
                miniCIdx = vertexLocToIdx[cKey];
            } else if (vertexLocToIdx.count(cRevKey)) {
                miniCIdx = vertexLocToIdx[cRevKey];
            } else {
                newVertices.push_back(midPointC);
                miniCIdx = newVertices.size() - 1;
                vertexLocToIdx[cKey] = miniCIdx;
            }

            int miniDIdx;
            std::string dKey = std::to_string(face.vertexIndices[2]);
            if (vertexLocToIdx.count(dKey)) {
                miniDIdx = vertexLocToIdx[dKey];
            } else {
                newVertices.push_back(nextHe.vertices[1]);
                miniDIdx = newVertices.size() - 1;
                vertexLocToIdx[dKey] = miniDIdx;
            }

            if (oldVertexNeighbors.count(miniDIdx)) {
                oldVertexNeighbors[miniDIdx].push_back(nextHe.vertices[0]);
            } else {
                oldVertexNeighbors[miniDIdx] = std::vector<glm::vec3>(1, nextHe.vertices[0]);
            }

            int miniEIdx;
            std::string eKey = std::to_string(face.vertexIndices[2]) + "-" + std::to_string(face.vertexIndices[0]);
            std::string eRevKey = std::to_string(face.vertexIndices[0]) + "-" + std::to_string(face.vertexIndices[2]);
            if (vertexLocToIdx.count(eKey)) {
                miniEIdx = vertexLocToIdx[eKey];
            } else if (vertexLocToIdx.count(eRevKey)) {
                miniEIdx = vertexLocToIdx[eRevKey];
            } else {
                newVertices.push_back(midPointE);
                miniEIdx = newVertices.size() - 1;
                vertexLocToIdx[eKey] = miniEIdx;
            }

            int miniFIdx;
            std::string fKey = std::to_string(face.vertexIndices[0]);
            if (vertexLocToIdx.count(fKey)) {
                miniFIdx = vertexLocToIdx[fKey];
            } else {
                newVertices.push_back(nextToNextHe.vertices[1]);
                miniFIdx = newVertices.size() - 1;
                vertexLocToIdx[fKey] = miniFIdx;
            }

            if (oldVertexNeighbors.count(miniFIdx)) {
                oldVertexNeighbors[miniFIdx].push_back(nextToNextHe.vertices[0]);
            } else {
                oldVertexNeighbors[miniFIdx] = std::vector<glm::vec3>(1, nextToNextHe.vertices[0]);
            }

            // The four new faces
            newFaces.push_back(TrimeshFace( miniAIdx, miniBIdx, miniCIdx ));
            newFaces.push_back(TrimeshFace( miniCIdx, miniDIdx, miniEIdx ));
            newFaces.push_back(TrimeshFace( miniEIdx, miniFIdx, miniAIdx ));
            newFaces.push_back(TrimeshFace( miniAIdx, miniCIdx, miniEIdx ));
        }

        // Update old coordinates in newVertices using existing old coordinates
        for (const auto& vertexPair : oldVertexNeighbors) {
            int vertexIndex = vertexPair.first;
            const vector<glm::vec3>& neighborOldVertices = vertexPair.second;
            int n = neighborOldVertices.size();

            double beta;
            if (n == 6) {
                beta = 3.0 / 16.0;
            } else {
                double alpha = (4.0 - 2.0 * cos(2.0 * M_PI / n)) / 9.0;
                beta = (1.0 - alpha) / n;
            }

            glm::vec3& vertex = newVertices[vertexIndex];
            vertex.x = (1.0 - n * beta) * vertex.x + beta * std::accumulate(neighborOldVertices.begin(), neighborOldVertices.end(), 0.0, sumXCoords);
            vertex.y = (1.0 - n * beta) * vertex.y + beta * std::accumulate(neighborOldVertices.begin(), neighborOldVertices.end(), 0.0, sumYCoords);
            vertex.z = (1.0 - n * beta) * vertex.z + beta * std::accumulate(neighborOldVertices.begin(), neighborOldVertices.end(), 0.0, sumZCoords);
        }

        currentVertices = std::move(newVertices);
        currentFaces = std::move(newFaces);
    }
    return std::make_pair(currentVertices, currentFaces);
}

string LoopSubdiv::doSubdivision(std::string filename, int iter) {
    opfile = filename;
    std::stringstream ss;

    if (iter > 0)
    {
        opfile = filename + "_op" + std::to_string(iter) + ".out";
        printf("\nPerforming Loop Subdivision\n");
        ss << "\nOriginal Mesh Info:\n" << vertices.size() << " vertices, " << trimeshFaces.size() << " faces\n";
        
        std::pair<std::vector<glm::vec3>, std::vector<TrimeshFace> > subdivided = subdivLoop(vertices, trimeshFaces, iter);
        ss << "\nSubdivided Mesh Info:\n" << subdivided.first.size() << " vertices, " << subdivided.second.size() << " faces\n";

        //printf("\nAfter subdivision \n");
        printSubdividedMesh(subdivided.first, subdivided.second);
    }
    string log = ss.str();
    displayScene();
    return log;
}

void LoopSubdiv::displayScene() {
    // Path to the executable
    std::string executable = "./render";

    // Arguments to pass to the executable
    std::string arg1 = opfile;

    // Construct the command string with arguments
    std::string command = executable + " " + arg1 + " 1";

    int result = system(command.c_str());

    // Check the return value
    if (result != 0)
        std::cout << "\nFailed to run the executable." << std::endl;
}