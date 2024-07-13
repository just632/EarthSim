#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class Shader {
public:
    Shader(){
        Shader("simple","simple");
    };
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    void use();
    GLuint getProgramID();
    GLint getUniformLocation(const std::string& name);

private:
    GLuint programID;
    std::unordered_map<std::string, GLint> uniformLocations;
    std::string loadShaderSource(const std::string& filePath);
    void compileShader(const char* source, GLuint shaderID);
};

#endif // SHADER_H
