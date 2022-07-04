#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <glew.h>
#include <math.h>
#include <glut.h>
#include <time.h>
#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

bool mouse_down = false;
int mouse_x, mouse_y;
int win_w, win_h;
bool changed_to_white = false;
static void RenderSceneCB();

class Vertex {
public:
    float x, y, z, r, g, b;
    float textX, textY;
    Vertex() {
        x = y = z = 0.0f;
        r = g = b = 1.0f;
        textX = textY = 0.0f;
    }
    Vertex(float _x, float _y, float _z, float _r, float _g, float _b, float _textX, float _textY) {
        x = _x;
        y = _y;
        z = _z;
        r = _r;
        g = _g;
        b = _b;
        textX = _textX;
        textY = _textY;
    }
    void setToWhite() {
        r = b = g = 1.0f;
    }
    
    float getTextX() {
        return textX;
    }

    float getTextY() {
        return textY;
    }

    void setTexture(float _textX, float _textY) {
        textX = _textX;
        textY = _textY;
    }

    bool shouldMoveVertex() {
        double oglx = -1.0 + 2.0 * float(mouse_x) / win_w;
        double ogly = 1.0 - 2.0 * float(mouse_y) / win_h;
        double distance = sqrt(pow(x - oglx, 2) + pow(y - ogly, 2));
        return (distance <= 1e-2);
    }

    void MoveVertex() {
        double oglx = -1.0 + 2.0 * float(mouse_x) / win_w;
        double ogly = 1.0 - 2.0 * float(mouse_y) / win_h;
        x = float(oglx);
        y = float(ogly);
    }

    void updateTexture(float offset_x, float offset_y) {
        textX += offset_x;
        textY += offset_y;
    }
    
};

const int N = 3;
Vertex vertexVector[4 * N]{};
GLuint VBO, VAO;
int pickingId = -1;
unsigned int texture;
int tw, th, tChannels;
float Xoffset = 0.125f;
float Yoffset = 0.1f;
int curr_question = 1;


static void adjustVertexData() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexVector), &vertexVector[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void RenderSceneCB() {
    adjustVertexData();
    int vertexColorLocation = glGetUniformLocation(ShaderProgram, "ourColor");
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawArrays(GL_QUADS, 0, 4 * N);
    glutSwapBuffers();
}

static void CreateVertexBuffer() {
    vertexVector[0] = Vertex(-0.9f, -0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0, 0.1f);
    vertexVector[1] = Vertex(-0.4f, -0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.125f, 0.1f);
    vertexVector[2] = Vertex(-0.4f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.125f, 0.0f);
    vertexVector[3] = Vertex(-0.9f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    vertexVector[4] = Vertex(-0.3f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.1f);
    vertexVector[5] = Vertex(0.2f, -0.25, 0.0f, 0.0f, 1.0f, 0.2f, 0.125f, 0.1f);
    vertexVector[6] = Vertex(0.2f, 0.25, 0.0f, 0.0f, 1.0f, 0.0f, 0.125f, 0.0f);
    vertexVector[7] = Vertex(-0.3f, 0.25, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);

    vertexVector[8] = Vertex(0.3f, -0.25f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.1f);
    vertexVector[9] = Vertex(0.8f, -0.25f, 0.0f, 0.0f, 0.0f, 1.0f, 0.125f, 0.1f);
    vertexVector[10] = Vertex(0.8f, 0.25f, 0.0f, 0.0f, 0.0f, 1.0f, 0.125f, 0.0f);
    vertexVector[11] = Vertex(0.3f, 0.25f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);

    // carga de texturas
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int tw, th, nrChannels;
    unsigned char* data = stbi_load("texture.jpg", &tw, &th, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tw, th, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cout << "Failed to load texture" << endl;
    }
    stbi_image_free(data);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexVector), vertexVector, GL_STREAM_DRAW);
    // atributo de posicion
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // atributo de color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // atributo de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glUseProgram(ShaderProgram);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        mouse_down = (state == GLUT_DOWN);
        glutPostRedisplay();
        if (state == GLUT_UP) {
            pickingId = -1;
        }
    }
    mouse_x = x;
    mouse_y = y;
}

void motion(int x, int y) {
    if (mouse_down) glutPostRedisplay();
    mouse_x = x;
    mouse_y = y;
    for (int i = 0; i < 4 * N; i++) {
        if (pickingId == -1 && vertexVector[i].shouldMoveVertex()) {
            pickingId = i;
            break;
        }
        if (pickingId == i) {
            vertexVector[i].MoveVertex();
        }
    }
}

void setStartingTextures() {
    vertexVector[0].setTexture(0.0f, 0.1f);
    vertexVector[1].setTexture(0.125f, 0.1f);
    vertexVector[2].setTexture(0.125f, 0.0f);
    vertexVector[3].setTexture(0.0f, 0.0f);

    vertexVector[4].setTexture(0.0f, 0.1f);
    vertexVector[5].setTexture(0.125f, 0.1f);
    vertexVector[6].setTexture(0.125f, 0.0f);
    vertexVector[7].setTexture(0.0f, 0.0f);

    vertexVector[8].setTexture(0.0f, 0.1f);
    vertexVector[9].setTexture(0.125f, 0.1f);
    vertexVector[10].setTexture(0.125f, 0.0f);
    vertexVector[11].setTexture(0.0f, 0.0f);
}

void updateQuestionQuad(float ofX, float ofY) {
    vertexVector[0].updateTexture(ofX, ofY);
    vertexVector[1].updateTexture(ofX, ofY);
    vertexVector[2].updateTexture(ofX, ofY);
    vertexVector[3].updateTexture(ofX, ofY);
}

void updateAnswerQuad(unsigned char stage) {
    if (stage == 'i') {
        vertexVector[4].updateTexture(0.0f, Yoffset * 9);
        vertexVector[5].updateTexture(0.0f, Yoffset * 9);
        vertexVector[6].updateTexture(0.0f, Yoffset * 9);
        vertexVector[7].updateTexture(0.0f, Yoffset * 9);
    }
    else if (stage == 'f') {
        vertexVector[4].setTexture(vertexVector[0].getTextX(), vertexVector[0].getTextY());
        vertexVector[5].setTexture(vertexVector[1].getTextX(), vertexVector[1].getTextY());
        vertexVector[6].setTexture(vertexVector[2].getTextX(), vertexVector[2].getTextY());
        vertexVector[7].setTexture(vertexVector[3].getTextX(), vertexVector[3].getTextY());

        vertexVector[4].updateTexture(Xoffset * 2, 0.0f);
        vertexVector[5].updateTexture(Xoffset * 2, 0.0f);
        vertexVector[6].updateTexture(Xoffset * 2, 0.0f);
        vertexVector[7].updateTexture(Xoffset * 2, 0.0f);
    }
}

void updateTopQuad(float ofX, float ofY) {
    // reset the akinator cycle
    if (vertexVector[8].getTextX() + ofX >= 1) ofX = -6 * ofX;
    // set topQuad texture coordinates (akinator images)
    vertexVector[8].updateTexture(ofX, ofY);
    vertexVector[9].updateTexture(ofX, ofY);
    vertexVector[10].updateTexture(ofX, ofY);
    vertexVector[11].updateTexture(ofX, ofY);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == '0' && changed_to_white == false || key == 13) {
        changed_to_white = true;
        for (int i = 0; i < 4 * N; i++) {
            vertexVector[i].setToWhite();
        }
        curr_question = 1;
        setStartingTextures();
        updateAnswerQuad('i');
        updateQuestionQuad(0.0f, Yoffset * 1);
        updateTopQuad(Xoffset, 0.0f);

    }
    if (key == '1' && changed_to_white) {
        if (curr_question < 4) {
            updateQuestionQuad(Xoffset, 0.0f);
            curr_question++;
        }
        else if (curr_question == 4) {
            updateQuestionQuad(Xoffset, 0.0f);
            updateAnswerQuad('f');
            curr_question++;
        }
        updateTopQuad(Xoffset, 0.0f);
    }
    else if (key == '2' && changed_to_white) {
        if (curr_question == 1) {
            updateQuestionQuad(Xoffset, Yoffset * 4);
            curr_question++;
        }
        else if (curr_question == 2) {
            updateQuestionQuad(Xoffset, Yoffset * 2);
            curr_question++;
        }
        else if (curr_question == 3) {
            updateQuestionQuad(Xoffset, Yoffset);
            curr_question++;
        }
        else if (curr_question == 4) {
            updateQuestionQuad(Xoffset * 2, 0.0f);
            updateAnswerQuad('f');
            curr_question++;
        }
        updateTopQuad(Xoffset, 0.0f);
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    win_w = w;
    win_h = h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2, 2, -2, 2);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    int win = glutCreateWindow("Entrega 1");
    printf("window id: %d\n", win);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(200, 100);

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }
    GLclampf Red = 0.0f, Green = 0.0f, Blue = 0.0f, Alpha = 0.0f;
    glClearColor(Red, Green, Blue, Alpha);
    CreateVertexBuffer();
    CompileShaders();
    glutDisplayFunc(RenderSceneCB);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}