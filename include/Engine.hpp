#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Utils/Shader.hpp"
#include "Utils/Camera.hpp"
#include "Utils/wgs84.hpp"
#include "Objects/Object.hpp"
#include "Objects/Ellipsoid.hpp"
#include "Objects/Cube.hpp"
// #include "Objects/Rock.hpp"
#include "Utils/Console.hpp"
// #include "Earth.hpp"

#define COMMAND (const std::string &args)
#define COMMAND_MAP std::map<std::string, std::function<std::string(const std::string &)>>

/* NOTE: why comented code dosnt work

class Timer {
public:
    Timer() : deltaTime(0.0f), previousTime(glfwGetTime()), currentTime(0.0f) {}

    float getDeltaTime() {
        return deltaTime;
    }

    void updateDeltaTime() {
        currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;
        std::cout << "Update DeltaTime: " << deltaTime << std::endl;
    }

private:
    float deltaTime;
    float previousTime;
    float currentTime;
};
*/

class Timer
{
public:
    static Timer &getInstance()
    {
        static Timer instance;
        return instance;
    }

    Timer(const Timer &) = delete;
    void operator=(const Timer &) = delete;

    float getDeltaTime()
    {
        return deltaTime;
    }

    void updateDeltaTime()
    {
        currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;
    }

private:
    Timer() : deltaTime(0.0f), previousTime(glfwGetTime()), currentTime(0.0f) {}

    float deltaTime;
    float previousTime;
    float currentTime;
};

class Engine
{
public:
    static Engine &getInctance()
    {
        static Engine INSTANCE;
        return INSTANCE;
    }
    ~Engine();
    void run();
    void switchCameraView(int viewIndex);
    void passInputToView(char input);

    GLFWwindow *getWindow()
    {
        return window;
    }

    std::shared_ptr<ConsoleBuffer> getConsole()
    {
        return console;
    }

    void drawObjects()
    {
        // Update and draw all flying objects
        for (auto obj : Objects)
        {
            obj->update();
            obj->draw();
        }
    }

    void drawConsole()
    {
        if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_PRESS && !console->isOpen)
            console->isOpen = true;

        console->render();
    }
    void drawCamera()
    {
        if (!console->isOpen)
            // Update and apply camera views
            getCurrentCamera()->update();

        getCurrentCamera()->apply();
    }

    bool isConsoleOpen()
    {
        return console->isOpen;
    }

    /*
        this method returns a refrence to a loaded shader
        if shader is not loaded the engine tries to load one
        shader src code is stored ate /shaders in the following format

        {name}.fragment.shader for a fragment shader
        {name}.vertex.shader for a vertex shader

        if shader not found throws an exception
    */
    std::shared_ptr<Shader> getShader(char *name);

    std::shared_ptr<Camera> getCurrentCamera()
    {
        return cameraViews[currentCameraViewIndex];
    };

    std::string currentCameraPos()
    {
        std::ostringstream oss;
        auto cam = getCurrentCamera();
        auto pos = cam->getPosition();
        oss << "x:" << roundToOneDecimal(pos.x)
            << "|y:" << roundToOneDecimal(pos.y)
            << "|z:" << roundToOneDecimal(pos.z);
        return oss.str();
    }

    std::string currentCameraOrent()
    {
        std::ostringstream oss;
        auto cam = getCurrentCamera();

        oss << "yaw:" << roundToOneDecimal(cam->getYaw())
            << "|pitch:" << roundToOneDecimal(cam->getPitch());

        return oss.str();
    }

    std::string currentObjectCount()
    {
        std::ostringstream oss;

        oss << "obj:" << roundToOneDecimal(Objects.size());

        return oss.str();
    }

    // void addObject(std::shared_ptr<Object> obj)
    // {
    //     Objects.push_back(obj);
    // }

    float WindoAspectRatio;
    float windowWidth;
    float windowHight;

    COMMAND_MAP getCommands()
    {
        return commands;
    }

    std::vector<float> ElipsoideVertices;
    GLuint ElipsoideVAO, ElipsoideVBO;

    float getDeltaTime()
    {
        return timer.getDeltaTime();
    }

    void updateDeltaTime()
    {
        timer.updateDeltaTime();
    }

    void addObject(std::shared_ptr<Object> object)
    {
        Objects.push_back(object);
    }

private:
    Timer &timer = Timer::getInstance();

    Engine();

    void generateEllipsoid(int slices, int stacks)
    {
        for (int i = 0; i <= stacks; ++i)
        {
            float theta = i * M_PI / stacks;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (int j = 0; j <= slices; ++j)
            {
                float phi = j * 2 * M_PI / slices;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                float x = cosPhi * sinTheta;
                float y = sinPhi * sinTheta;
                float z = cosTheta;

                float xr = WGS84::A * x;
                float yr = WGS84::A * y;
                float zr = WGS84::B * z;

                ElipsoideVertices.push_back(xr);
                ElipsoideVertices.push_back(yr);
                ElipsoideVertices.push_back(zr);
            }
        }
        std::cout << std::endl;
    }

    void initWindow();
    void initOpenGL();
    void mainLoop();
    void cleanup();
    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    GLFWwindow *window;

    std::shared_ptr<ConsoleBuffer> console;

    std::map<std::string, std::shared_ptr<Shader>> shaders;
    std::vector<std::shared_ptr<Object>> Objects;
    std::vector<std::shared_ptr<Camera>> cameraViews;
    int currentCameraViewIndex = 0;

    // Earth& earth;

    COMMAND_MAP commands = {
        {"ping", [] COMMAND
         {
             return "pong";
         }},
        {"add", [] COMMAND
         {
             std::istringstream iss(args);
             std::string a_str, b_str;
             char comma;
             std::ostringstream oss;
             if (std::getline(iss, a_str, ',') && std::getline(iss, b_str, ')'))
             {
                 try
                 {
                     float a = std::stof(a_str);
                     float b = std::stof(b_str);
                     oss << a + b;
                 }
                 catch (const std::invalid_argument &e)
                 {
                     oss << "a: " << a_str << " b: " << b_str << " Invalid number format. Use add(a,b) with valid numbers.";
                 }
                 catch (const std::out_of_range &e)
                 {
                     oss << "Number out of range. Use add(a,b) with valid numbers.";
                 }
             }
             else
             {
                 oss << "a: " << a_str << " b: " << b_str << "Invalid format. Use add(a,b)";
             }
             return oss.str();
         }},
        {"echo", [] COMMAND
         {
             std::ostringstream oss;
             oss << args;
             return oss.str();
         }},
        {"cube", [] COMMAND
         {
             std::ostringstream oss;
             try
             {
                 Engine &engine = Engine::getInctance();
                 std::shared_ptr<Camera> currentCamera = engine.getCurrentCamera();
                 engine.addObject(std::make_shared<Cube>(&engine,glm::vec3(0.f,70.f,0.f)));
             }
             catch (const std::exception &e)
             {
                 oss << e.what() << '\n';
             }
             return oss.str();
         }},
        {"help", [] COMMAND
         {
             std::ostringstream oss;
             oss << "ping -> returns pong\n";
             oss << "add(a,b) -> adds two floats\n";
             oss << "echo(msg) -> returns msg\n";
             oss << "rock -> spawns rock at current position\n";
             return oss.str();
         }}};

    float roundToOneDecimal(const float number)
    {
        return std::round(number * 10) / 10;
    }
};

#endif // ENGINE_H
