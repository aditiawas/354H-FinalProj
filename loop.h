#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unordered_set>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <array>
#include <string>
#include <vector>
#include <string>
#include "subdivobjects.h"
#include "render.h"

using namespace std;

class HalfEdge {
public:
    std::array<glm::vec3, 2> vertices;
    std::string pairHalfEdgeKey;
    std::array<int, 3> face;
    std::array<glm::vec3, 2> nextHalfEdgeVertices;
};

struct VertexHash {
    size_t operator()(const glm::vec3& v) const {
        size_t seed = 0;
        seed ^= std::hash<double>()(v.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<double>()(v.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<double>()(v.z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

struct LoopSubdiv {
public:
    std::string opfile;
    vector<glm::vec3> vertices;
    vector<TrimeshFace> trimeshFaces;

    LoopSubdiv(vector<glm::vec3> vertices, vector<TrimeshFace> trimeshFaces) {
        this->vertices = vertices;
        this->trimeshFaces = trimeshFaces;
    }

    std::pair<vector<glm::vec3>, std::vector<TrimeshFace> > subdivLoop(const vector<glm::vec3>& vertices, const std::vector<TrimeshFace>& faces, int iterations);
    string doSubdivision(std::string filename, int iter);
    void printSubdividedMesh(std::vector<glm::vec3> v, std::vector<TrimeshFace> f);
    void displayScene(); // Retained
};