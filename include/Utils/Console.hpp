#pragma once
#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#define SCALE 0.4


#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <FlyingObjects/Rock.hpp>
#include <glm/gtc/quaternion.hpp> 
#include "Utils/Camera.hpp"
class Engine;

struct Character {
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint Advance;
};

class ConsoleBuffer {
public:
    ConsoleBuffer(Engine& engine, int maxLines);

    void addInput(int c, bool shift);
    void render();
    void load();

    bool isOpen = false;
private:
    std::map<std::string, std::function<std::string(const std::string &)>> commands;
    Engine& engine;
    std::vector<std::string> lines;
    std::string input;
    int maxLines;
    GLuint VAO, VBO;
    GLuint shaderProgram;
    glm::mat4 projection;
    float lineSize = 48.f;
    std::map<GLchar, Character> Characters;

    void initFreeType();
    int addShiftInput(int c);
    void addMessage(const std::string &message);
    std::string processCommand(const std::string &command);
    std::string trim(const std::string &str);
    std::string extractCommand(const std::string &str);
    void renderText(const std::string &text, float x, float y, float scale);
};

#endif // CONSOLE_HPP
