#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#endif

#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <functional>
#include <cstdlib>
#include "render.h"
#include "subdivobjects.h"

OpenGLWindow* g_window = nullptr;
float zoomFactor = 1.0f; // Initial zoom factor

void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
    int i = static_cast<int>(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
}

void OpenGLWindow::InitGL() {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    GLfloat specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    //glEnable(GL_CULL_FACE); // Enable face culling

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, (GLfloat)kWindowWidth / (GLfloat)kWindowHeight, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void OpenGLWindow::DrawShape() {
    //glEnable(GL_CULL_FACE);

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glDisable(GL_CULL_FACE); // Disable face culling for the wireframe mesh //ADITI: done

    if (!quadFaces.empty()) {
        glBegin(GL_LINES);
        for (const QuadFace* face : quadFaces) {
            int idx1 = face->v1;
            int idx2 = face->v2;
            int idx3 = face->v3;
            int idx4 = face->v4;

            glVertex3fv(glm::value_ptr(vertices[idx1]));
            glVertex3fv(glm::value_ptr(vertices[idx2]));

            glVertex3fv(glm::value_ptr(vertices[idx2]));
            glVertex3fv(glm::value_ptr(vertices[idx3]));

            glVertex3fv(glm::value_ptr(vertices[idx3]));
            glVertex3fv(glm::value_ptr(vertices[idx4]));

            glVertex3fv(glm::value_ptr(vertices[idx4]));
            glVertex3fv(glm::value_ptr(vertices[idx1]));
        }
        glEnd();
    } else {
        glBegin(GL_LINES);
        for (const TrimeshFace* face : faces) {
            glVertex3fv(glm::value_ptr(vertices[face->vertexIndices[0]]));
            glVertex3fv(glm::value_ptr(vertices[face->vertexIndices[1]]));

            glVertex3fv(glm::value_ptr(vertices[face->vertexIndices[1]]));
            glVertex3fv(glm::value_ptr(vertices[face->vertexIndices[2]]));

            glVertex3fv(glm::value_ptr(vertices[face->vertexIndices[2]]));
            glVertex3fv(glm::value_ptr(vertices[face->vertexIndices[0]]));
        }
        glEnd();
    }

    glDisable(GL_LIGHTING); // Disable lighting
    //glColor3f(0.1f, 0.5f, 1.0f); // Set a fixed color

        if (!quadFaces.empty()) {
        glBegin(GL_QUADS);
        for (size_t i = 0; i < quadFaces.size(); ++i) {
            const QuadFace* face = quadFaces[i];
            int idx1 = face->v1;
            int idx2 = face->v2;
            int idx3 = face->v3;
            int idx4 = face->v4;

            // Generate a unique color for each face
            float hue = static_cast<float>(i) / quadFaces.size();
            float saturation = 0.8f;
            float value = 0.8f;
            float r, g, b;
            HSVtoRGB(hue, saturation, value, r, g, b);
            glColor3f(r, g, b);

            glVertex3fv(glm::value_ptr(vertices[idx1]));
            glVertex3fv(glm::value_ptr(vertices[idx2]));
            glVertex3fv(glm::value_ptr(vertices[idx3]));
            glVertex3fv(glm::value_ptr(vertices[idx4]));
        }
        glEnd();
    } else {
        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < faces.size(); ++i) {
            const TrimeshFace* face = faces[i];
            int idx1 = face->vertexIndices[0];
            int idx2 = face->vertexIndices[1];
            int idx3 = face->vertexIndices[2];

            // Generate a unique color for each face
            float hue = static_cast<float>(i) / faces.size();
            float saturation = 0.8f;
            float value = 0.8f;
            float r, g, b;
            HSVtoRGB(hue, saturation, value, r, g, b);
            glColor3f(r, g, b);

            glVertex3fv(glm::value_ptr(vertices[idx1]));
            glVertex3fv(glm::value_ptr(vertices[idx2]));
            glVertex3fv(glm::value_ptr(vertices[idx3]));
        }
        glEnd();
    }

    glEnable(GL_LIGHTING); // Re-enable lighting

    //glDisable(GL_CULL_FACE);
}

void OpenGLWindow::DrawGLScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    light_position[0] = cameraX;
    light_position[1] = cameraY;
    light_position[2] = cameraZ;
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Apply zoom factor
    glTranslatef(0.0f, 0.0f, -5.0f * zoomFactor); //ADITI: done

    gluLookAt(cameraX, cameraY, cameraZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);

    DrawShape();

    glutSwapBuffers();
}

void OpenGLWindow::onMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouse_down = true;
        xDown = x;
        yDown = y;
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        mouse_down = false;
    }
}

void OpenGLWindow::onMotion(int x, int y) {
    if (mouse_down) {
        int dx = x - xDown;
        int dy = y - yDown;

        xrot += dy / 5.0f;
        yrot += dx / 5.0f;

        xDown = x;
        yDown = y;

        glutPostRedisplay();
    }
}

void OpenGLWindow::onKeyboard(unsigned char key, int x, int y) {
    if (key == 27) {  // ASCII code for the Escape key
        onClose();
    }
    switch (key) {
        case 'w':
            cameraY += translationSpeed;
            break;
        case 's':
            cameraY -= translationSpeed;
            break;
        case 'a':
            cameraX -= translationSpeed;
            break;
        case 'd':
            cameraX += translationSpeed;
            break;
        case 'r':
            cameraZ += translationSpeed;
            break;
        case 'f':
            cameraZ -= translationSpeed;
            break;
        case 'o':
            zoomFactor *= 1.5f;
            break;
        case 'i':
            zoomFactor /= 1.5f;
            break;
        default:
            break;

        light_position[0] = cameraX;
        light_position[1] = cameraY;
        light_position[2] = cameraZ;
    }

    glutPostRedisplay();
}

void OpenGLWindow::reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

void OpenGLWindow::onClose() {
    glutDestroyWindow(glutGetWindow());
    exit(0);
}

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <renderQuads>" << std::endl;
        return 1;
    }

    bool renderQuads = std::stoi(argv[2]) != 1;
    OpenGLWindow window(argc, argv, argv[1], renderQuads);

    window.Run();

    return 0;
}