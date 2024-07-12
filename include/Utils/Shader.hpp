#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    void use();
    GLuint getProgramID();

private:
    GLuint programID;
    std::string loadShaderSource(const std::string& filePath);
    void compileShader(const char* source, GLuint shaderID);
};

#endif // SHADER_H
