#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class Shader
{
public:

    GLint getUniformLocation(const std::string &locationName);

    Shader():
    Shader("simple","simple")
    {
    };
    Shader(char* vertexName, char* fragmentName):
    vertexName(vertexName),fragmentName(fragmentName)
    {

    };
    ~Shader()
    {
        glDeleteProgram(programID);
    };
    void use()
    {
        glUseProgram(programID);
    };
    GLuint getProgramID()
    {
        return programID;
    };

    void load(){
        if(loaded)return;
        char vertexPath[256];
        char fragmentPath[256];
        sprintf(vertexPath, "shaders/%s.vs", vertexName);
        sprintf(fragmentPath, "shaders/%s.fs", fragmentName);

        std::string vertexCode = loadShaderSource(vertexPath);
        std::string fragmentCode = loadShaderSource(fragmentPath);

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        compileShader(vertexCode.c_str(), vertexShader);
        compileShader(fragmentCode.c_str(), fragmentShader);

        programID = glCreateProgram();
        glAttachShader(programID, vertexShader);
        glAttachShader(programID, fragmentShader);
        glLinkProgram(programID);

        GLint success;
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetProgramInfoLog(programID, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                      << infoLog << "on file : " << vertexPath << "\n and : " << fragmentPath << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        loaded=true;
    }


        // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &locationName, bool value) const
    {         
        glUniform1i(glGetUniformLocation(programID, locationName.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &locationName, int value) const
    { 
        glUniform1i(glGetUniformLocation(programID, locationName.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &locationName, float value) const
    { 
        glUniform1f(glGetUniformLocation(programID, locationName.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &locationName, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(programID, locationName.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &locationName, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(programID, locationName.c_str()), x, y); 
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &locationName, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(programID, locationName.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &locationName, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(programID, locationName.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &locationName, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(programID, locationName.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &locationName, float x, float y, float z, float w) const
    { 
        glUniform4f(glGetUniformLocation(programID, locationName.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &locationName, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(programID, locationName.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &locationName, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(programID, locationName.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &locationName, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(programID, locationName.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

private:
    GLuint programID = 0;
    char* vertexName;
    char* fragmentName;
    bool loaded = false;
    std::unordered_map<std::string, GLint> uniformLocations;
    std::string loadShaderSource(const std::string &filePath)
    {
        std::ifstream shaderFile;
        std::stringstream shaderStream;

        shaderFile.open(filePath);
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();

        return shaderStream.str();
    };
    void compileShader(const char *source, GLuint shaderID)
    {
        glShaderSource(shaderID, 1, &source, NULL);
        glCompileShader(shaderID);

        GLint success;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
    };
};

#endif // SHADER_H
