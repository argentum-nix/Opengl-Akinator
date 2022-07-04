#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <string>
#include <glew.h>
#include <glut.h>
using namespace std;

extern GLuint ShaderProgram;
bool ReadFile(const char* fileName, string& outFile);
void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
void CompileShaders();

#endif  /* UTILS_H */