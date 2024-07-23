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
#include "Cameras/Camera.hpp"
#include "Utils/Shader.hpp"
#include "static/wgs84.hpp"
#include "Objects/Object.hpp"
#include "Objects/Ellipsoid.hpp"

#include "Objects/Triangle.hpp"
#include "Objects/Cube.hpp"
#include "Utils/Console.hpp"
#include "Utils/Window.hpp"
#include "Utils/Timer.hpp"
#define COMMAND_ARGS (const std::string &args)
using namespace Utils;

class Engine
{
public:
        Engine(const Engine &obj) = delete;
        static Engine *getInstance()
        {
            if (instance != nullptr)
            {
                return instance;
            }
            instance = new Engine();
            return instance;
        }
    ~Engine();
    void run();
    void switchCameraView(int viewIndex);
    void passInputToView(char input);

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
        if (window->keyPressed( GLFW_KEY_SLASH) == GLFW_PRESS && !console->isOpen)
            console->isOpen = true;
        auto cam = getCurrentCamera();
        console->updateObjectCount(Objects.size());
        console->updateCameraYaw(cam->getYaw());
        console->updateCameraPitch(cam->getPitch());
        console->updateCameraPos(cam->getPosition());
        console->render();
    }
    void updateCamera()
    {
        if (!console->isOpen)
            // Update and apply camera views
            getCurrentCamera()->update();
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

    void addObject(std::shared_ptr<Object> object)
    {
        Objects.push_back(object);
    }

private:

    static Engine *instance;
    Timer* timer = Timer::getInstance(); 
    Window* window = Window::getInstance();
    Console* console = Console::getInstance();

    std::map<std::string, std::shared_ptr<Shader>> shaders;
    std::vector<std::shared_ptr<Object>> Objects;
    std::vector<std::shared_ptr<Camera>> cameraViews;
    int currentCameraViewIndex = 0;

    Engine();


    void setGlfwCallbacks()
    {
        glfwSetFramebufferSizeCallback(window->getWindow_ptr(), framebufferSizeCallback);
        glfwSetKeyCallback(window->getWindow_ptr(),keyCallback);
    }
    
    void mainLoop();
    void cleanup();
    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);


    void loadCommands()
    {
        console->addCommand("ping",[]COMMAND_ARGS
         {
             return "pong";
         });

        console->addCommand("add", []COMMAND_ARGS
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
         });
         console->addCommand("echo", []COMMAND_ARGS
         {
             std::ostringstream oss;
             oss << args;
             return oss.str();
         });
         console->addCommand("cube", []COMMAND_ARGS
         {
             std::ostringstream oss;
             try
             {
                 Engine *engine = Engine::getInstance();
                 std::shared_ptr<Camera> currentCamera = engine->getCurrentCamera();
                 auto pos = currentCamera->getPosition();
                 pos.z -= 5;
                 engine->addObject(std::make_shared<Cube>(pos));
                 oss << "created cube at (x:"<<pos.x<<",y:"<<pos.y<<",z:"<<pos.z;
             }
             catch (const std::exception &e)
             {
                 oss << e.what() << '\n';
             }
             return oss.str();
         });
         console->addCommand("help", []COMMAND_ARGS
         {
             std::ostringstream oss;
             oss << "ping -> returns pong\n";
             oss << "add(a,b) -> adds two floats\n";
             oss << "echo(msg) -> returns msg\n";
             oss << "rock -> spawns rock at current position\n";
             return oss.str();
         });
    }


};

#endif // ENGINE_H
