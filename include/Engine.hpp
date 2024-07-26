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
#include "Objects/Rock.hpp"
#define COMMAND_ARGS (const std::vector<std::string> &args)
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
        console->addCommand("add",[]COMMAND_ARGS
         {
            std::ostringstream oss;

            if(args.size()<2){
                oss<< "add requiers 2 or more args";

             return oss.str();}
            float result=0.0f;
            
            for(auto arg : args){
                 try
                 {
                    result+=std::stof(arg);
                 }
                 catch (const std::out_of_range &e)
                 {
                     oss << "Invalid number : "<<arg<<" at add command";
                     break;
                 }
            }
             
            oss<<"add -> "<<result;
            return oss.str();
         });
         console->addCommand("echo", []COMMAND_ARGS
         {
             std::ostringstream oss;
             for(auto arg:args)oss << arg;
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
                 oss << "created cube at (x:"<<pos.x<<",y:"<<pos.y<<",z:"<<pos.z<<")";
             }
             catch (const std::exception &e)
             {
                 oss << e.what() << '\n';
             }
             return oss.str();
         });
        console->addCommand("rock", []COMMAND_ARGS
         {
             std::ostringstream oss;
             try
             {
                 Engine *engine = Engine::getInstance();
                 std::shared_ptr<Camera> currentCamera = engine->getCurrentCamera();
                 auto pos = currentCamera->getPosition();
                 auto forward = currentCamera->getForward();
                 float force = 1;
                 if(args.size()>=1)force = std::stof(args[0]);
                 auto val = forward*force;
                 engine->addObject(std::make_shared<Rock>(pos,val,glm::quat(1.,0.,0.,0.)));
                 oss << "created rock at (x:"<<pos.x<<",y:"<<pos.y<<",z:"<<pos.z<<")";
             }
             catch (const std::exception &e)
             {
                 oss << e.what() << '\n';
             }
             return oss.str();
         });

         console->addCommand("q",[]COMMAND_ARGS{
            std::ostringstream oss;
            glfwSetWindowShouldClose(Window::getInstance()->getWindow_ptr(),true);
            oss << "exiting...";
            return oss.str();
         });

        console->addCommand("norm",[]COMMAND_ARGS{
            std::ostringstream oss;
            Engine *engine = Engine::getInstance();
            std::shared_ptr<Camera> currentCamera = engine->getCurrentCamera();
            auto pos = currentCamera->getPosition();
            auto geoPos = WGS84::toGeodetic(pos);
            auto lat = geoPos.x;
            auto lon = geoPos.y;
            auto norm = WGS84::surfaceNormal(lat,lon);
            oss << "norm("<<norm.x<<","<<norm.y<<","<<norm.z<<")";
            return oss.str();
         });
         console->addCommand("g",[]COMMAND_ARGS{
            std::ostringstream oss;
            Engine *engine = Engine::getInstance();
            std::shared_ptr<Camera> currentCamera = engine->getCurrentCamera();
            auto pos = currentCamera->getPosition();
            auto geoPos = WGS84::toGeodetic(pos);
            auto lat = geoPos.x;
            auto lon = geoPos.y;
            auto alt = geoPos.z;
            double g = WGS84::gravityAtHeight(lat, alt);
            // Get the surface normal at the current geodetic position
            glm::vec3 normal = WGS84::surfaceNormal(lat, lon);

            double forceMagnitude = g * 1.f;

            // The force is in the opposite direction of the normal
            double forceX = -forceMagnitude * normal.x;
            double forceY = -forceMagnitude * normal.y;
            double forceZ = -forceMagnitude * normal.z;
            oss << "g("<<forceX<<","<<forceY<<","<<forceZ<<")";
            return oss.str();
         });
         console->addCommand("front",[]COMMAND_ARGS{
            std::ostringstream oss;
            Engine *engine = Engine::getInstance();
            std::shared_ptr<Camera> currentCamera = engine->getCurrentCamera();
            auto forward = currentCamera->getForward();
            oss << "forward("<<forward.x<<","<<forward.y<<","<<forward.z<<")";
            return oss.str();
         });
         console->addCommand("time",[]COMMAND_ARGS{
            std::ostringstream oss;
            oss << "running for : "<<glfwGetTime();
            return oss.str();
         });
         console->addCommand("setSpeed",[]COMMAND_ARGS{
             std::ostringstream oss;
             try
             {
                 float timeMult = std::stof(args[0]);
                 Timer::setTimeMultiplier(timeMult);
                 oss << "set time multiplier";
             }
             catch (const std::exception &e)
             {
                 oss << e.what() << '\n';
             }
             return oss.str();
         });
         console->addCommand("addSpeed",[]COMMAND_ARGS{
             std::ostringstream oss;
             try
             {
                 float timeMult = std::stof(args[0]);
                 Timer::setTimeMultiplier(Timer::getTimeMultiplier() + timeMult);
                 oss << "added to time multiplier";
             }
             catch (const std::exception &e)
             {
                 oss << e.what() << '\n';
             }
             return oss.str();
         });
        console->addCommand("p",[]COMMAND_ARGS{
            std::ostringstream oss;
            if(Timer::getTimeMultiplier()==0.f){
                Timer::setTimeMultiplier(1.f);

                oss << "unpaused simulation";
            }else{

                Timer::setTimeMultiplier(0.f);

                oss << "paused simulation";
            }
            return oss.str();
         });
         console->addCommand("c", []COMMAND_ARGS
         {
             std::ostringstream oss;
             try
             {
                 Engine *engine = Engine::getInstance();
                 for(auto o:engine->Objects)o.reset();
                 engine->Objects.clear();
                 engine->addObject(std::make_shared<Ellipsoid>(glm::vec3(0.f),100,100));
                 oss << "cleared objects";
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
             oss << "add(a,b) -> adds two or more floats\n";
             oss << "echo(msg) -> returns msg\n";
             oss << "cube -> spawns cube at current position\n";
             oss << "rock -> spawns rock at current position\n";
             oss << "front -> prints actrive cameras forward vec\n";
             oss << "time -> print the uptime of app in seconds\n";
             oss << "g -> prints gravity vec at current position\n";
             oss << "c -> clear all objects\n";
             oss << "norm -> prints surface norm vec at current position";
             return oss.str();
         });
    }


};

#endif // ENGINE_H
