#ifndef RENDER_H
#define RENDER_H

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/gl.h>
#endif

#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <functional>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "subdivobjects.h"

using namespace std;

class OpenGLWindow; // Forward declaration
extern OpenGLWindow* g_window;

#define kWindowWidth 1000
#define kWindowHeight 800

class OpenGLWindow {
public:
    std::vector<glm::vec3> vertices;
    std::vector<TrimeshFace*> faces;
    std::vector<QuadFace*> quadFaces;

    OpenGLWindow(int argc, char** argv, std::string filename, bool renderQuads) : vertices(vertices), faces(faces), quadFaces(quadFaces) {
        glutInit(&argc, argv); //ADITI: check
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(kWindowWidth, kWindowHeight);
        glutInitWindowPosition(100, 100);
        glutCreateWindow(argv[0]);
        g_window = this;

        // Set the window close callback
        glutWMCloseFunc(onCloseWrapper);

        InitGL();
        std::vector<glm::vec3> vertices2;
        std::vector<TrimeshFace*> faces2;
        std::vector<QuadFace*> quadfaces2;

        if (renderQuads) {
            readQuad(filename, vertices2, quadfaces2);
            this->vertices = vertices2;
            this->quadFaces = quadfaces2;
        } else {
            readTrimesh(filename, vertices2, faces2);
            this->vertices = vertices2;
            this->faces = faces2;
        }

        DrawShape();

        glutDisplayFunc(DrawGLSceneWrapper); //ADITI: hacky way
        glutReshapeFunc(reshapeWrapper);
        glutMouseFunc(onMouseWrapper);
        glutMotionFunc(onMotionWrapper);
        glutKeyboardFunc(onKeyboardWrapper);
    }

    void Run() {
        glutMainLoop();
    }

    //void SetupScene();

    //reading the trimesh face from a file
    void readTrimesh(const std::string& filename, vector<glm::vec3> &vertices, vector<TrimeshFace*> &trimeshfaces )
    {   
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) 
        {
            std::istringstream iss(line);
            std::string token;
            iss >> token;
            if (token == "v") //take in the float values of the xyz positions of each vertices
            {
                float x, y, z;
                if (iss >> x >> y >> z) 
                {
                    //printf("Vertex: %5.2f %5.2f %5.2f \n",x,y,z);
                    vertices.push_back(glm::vec3(x,y,z));
                } 
                else 
                {
                    std::cerr << "Invalid vertex format in line: " << line << std::endl;
                }
            }
            else if (token == "f") //this line corresponds to a face line: tells you how to join the vertices
            {
                int a, b, c;
                if (iss >> a >> b >> c) 
                {
                    //std::cout << "Face: " << a << ", " << b << ", " << c << std::endl;
                    TrimeshFace* tempTrimeshFace = new TrimeshFace(a-1, b-1, c-1);
                    trimeshfaces.push_back(tempTrimeshFace);

                } 
                else 
                {
                    std::cerr << "Skipping this line: " << line << std::endl;
                }
            }
        }
        //printf("\n\nDone with readTrimesh...\n");
        file.close();

    }

    void readQuad(const std::string& filename, vector<glm::vec3> &vertices, vector<QuadFace*> &quadfaces )
    {   
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) 
        {
            std::istringstream iss(line);
            std::string token;
            iss >> token;
            if (token == "v") 
            {
                float x, y, z;
                if (iss >> x >> y >> z) 
                {
                    //printf("Vertex: %5.2f %5.2f %5.2f \n",x,y,z);
                    vertices.push_back(glm::vec3(x,y,z));
                } 
                else 
                {
                    std::cerr << "Invalid vertex format in line: " << line << std::endl;
                }
            } 
            else if (token == "f") 
            {
                int a, b, c, d;
                if (iss >> a >> b >> c >>d) 
                {
                    //std::cout << "Face: " << a << ", " << b << ", " << c << ", " <<d << std::endl;
                    QuadFace* tempQuadFace = new QuadFace(a-1,b-1,c-1,d-1);
                    quadfaces.push_back(tempQuadFace);

                } 
                else 
                {
                    std::cerr << "Invalid face format in line: " << line << std::endl;
                }
            } 
            else 
            {
                std::cerr << "Unknown token: " << token << " in line: " << line << std::endl;
            }
        }
        printf("Done with readQuad...");
        file.close();

    }

private:
    float xrot = 0.0f;
    float yrot = 0.0f;
    float xrotspeed = 0.0f;
    float yrotspeed = 0.0f;

    bool mouse_down = false;
    int xDown = 0, yDown = 0;
    float translationSpeed = 0.1f;

    float cameraX = 0.0f;
    float cameraY = 0.0f;
    float cameraZ = 1.0f;

    GLfloat light_position[4] = { cameraX, cameraY, cameraZ, 1.0f };

    void crossProduct(float v1x, float v1y, float v1z, float v2x, float v2y, float v2z, float result[3]) {
        result[0] = v1y * v2z - v1z * v2y;
        result[1] = v1z * v2x - v1x * v2z;
        result[2] = v1x * v2y - v1y * v2x;
    }

    void InitGL();

    void DrawShape();

    void DrawGLScene();

    void onMouse(int button, int state, int x, int y);

    void onMotion(int x, int y);

    void onKeyboard(unsigned char key, int x, int y);

    void reshape(int width, int height);
    
    void onClose();

    //ADITI: hacky way
    static void DrawGLSceneWrapper() {
        if (g_window != nullptr) {
            g_window->DrawGLScene();
        }
    }

    static void reshapeWrapper(int width, int height) {
        if (g_window != nullptr) {
            g_window->reshape(width, height);
        }
    }

    static void onCloseWrapper() {
        if (g_window != nullptr) {
            g_window->onClose();
        }
    }

    static void onMouseWrapper(int button, int state, int x, int y) {
        if (g_window != nullptr) {
            g_window->onMouse(button, state, x, y);
        }
    }

    static void onMotionWrapper(int x, int y) {
        if (g_window != nullptr) {
            g_window->onMotion(x, y);
        }
    }

    static void onKeyboardWrapper(unsigned char key, int x, int y) {
        if (g_window != nullptr) {
            g_window->onKeyboard(key, x, y);
        }
    }
};

#endif