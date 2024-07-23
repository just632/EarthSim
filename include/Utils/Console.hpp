#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/quaternion.hpp> 
#include "Utils/Shader.hpp"
#include "Utils/Window.hpp"
#include "Utils/Timer.hpp"
#define COMMAND std::function<std::string (const std::string &)>


struct Character {
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint Advance;
};

class Console {

public:
        Console(const Console &obj) = delete;
        static Console *getInstance()
        {
            if (instance != nullptr)
            {
                return instance;
            }
            instance = new Console();
            return instance;
        }

    ~Console() = default;

    void init();
    void render();
    void handleChar(int c, bool shift);

    void addCommand(std::string name,COMMAND handle){commands[name] = handle;}
    void updateObjectCount(int count){objectsCount=count;}
    void updateCameraYaw(float yaw){cameraYaw=yaw;}
    void updateCameraPitch(float pitch){cameraPitch=pitch;}
    void updateCameraPos(glm::vec3 pos){cameraPosition=pos;}

    bool isOpen=false;
    bool debugInfo=false;
private:
    static Console *instance;
    Console();

    const int maxLines = 10;
    const float lineSize = 48.f;
    const float scale = 0.4;

    float blinkTimer = 0.0f;
    bool showIndicator = true;
    const float blinkInterval = 0.5f; // Blinking interval in seconds


    int objectsCount=0;
    float cameraYaw=0;
    float cameraPitch=0;
    glm::vec3 cameraPosition=glm::vec3(0.f);

    std::map<std::string, COMMAND> commands;
    
    Shader shaderProgram = Shader("console","console");
    Window* window = Window::getInstance();
    Utils::Timer* timer = Utils::Timer::getInstance(); 
    std::vector<std::string> lines;
    std::vector<std::string> history;
    std::string tempInput; // Temporary storage for current input
    std::string input;
    int inputPos=0;
    int historyPos=0;

    GLuint VAO, VBO;
    glm::mat4 projection;
    std::map<GLchar, Character> Characters;

    void initFreeType();
    void addMessage(const std::string &message);
    void addInput(int c,bool shift);
    std::string processCommand(const std::string &command);
    std::string trim(const std::string &str);
    std::string extractCommand(const std::string &str);
    void renderText(const std::string &text, float x, float y, float scale);
    float renderCharacter(char c, float x, float y, float scale);
    float renderCharacter(char c, float x, float y, float scale,float shiftLeft);
    void renderTextWithIndicator(const std::string &text, float x, float y, float scale);



    std::string currentCameraPos()
    {
        std::ostringstream oss;
        oss << "x:" << roundToOneDecimal(cameraPosition.x)
            << "|y:" << roundToOneDecimal(cameraPosition.y)
            << "|z:" << roundToOneDecimal(cameraPosition.z);
        return oss.str();
    }

    std::string currentCameraOrent()
    {
        std::ostringstream oss;
        oss << "yaw:" << roundToOneDecimal(cameraYaw)
            << "|pitch:" << roundToOneDecimal(cameraPitch);

        return oss.str();
    }

    std::string currentObjectCount()
    {
        std::ostringstream oss;

        oss << "obj:" << objectsCount;

        return oss.str();
    }

        std::string currentTime()
    {
        std::ostringstream oss;

        oss << "time:" << roundToOneDecimal(timer->getTime());

        return oss.str();
    }

    int ShiftChar(int c) {
    switch (c) {
        case '`': return '~';
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case '\\': return '|';
        case ';': return ':';
        case '\'': return '"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        default: return c;
    }
}

    float roundToOneDecimal(const float number)
    {
        return std::round(number * 10) / 10;
    }

};