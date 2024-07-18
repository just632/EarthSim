#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Utils/Shader.hpp"
#include "Utils/Camera.hpp"
#include "FlyingObjects/FlyingObject.hpp"
#include "FlyingObjects/Rock.hpp"
#include "Utils/Console.hpp"
#include "Earth.hpp"


    #define COMMAND (const std::string &args)
    #define COMMAND_MAP std::map<std::string, std::function<std::string(const std::string &)>> 



class Engine {
public:
    static Engine& getInctance()
   {
    std::cout<<"getInctance"<<std::endl;
      static Engine INSTANCE;
      return INSTANCE;
   }
    ~Engine();
    void run();
    void switchCameraView(int viewIndex);
    void passInputToView(char input);

    GLFWwindow* getWindow(){
        return window;
    }


    /*
        this method returns a refrence to a loaded shader
        if shader is not loaded the engine tries to load one
        shader src code is stored ate /shaders in the following format

        {name}.fragment.shader for a fragment shader
        {name}.vertex.shader for a vertex shader

        if shader not found throws an exception
    */
    Shader& getShader(const std::string& name);

    Camera& getCurrentCamera(){
        return cameraViews[currentCameraViewIndex];
    };

    void addFlyingObject(FlyingObject& obj)
    {
        flyingObjects.push_back(obj);
    }

    double deltaTime;
    float WindoAspectRatio;
    int windowWidth;
    int windowHight;

    COMMAND_MAP getCommands(){
        return commands;
    }

private:

    double previousTime;
    double currentTime;
    std::vector<float> ElipsoideVertices;
    GLuint ElipsoideVAO, ElipsoideVBO;
    

    Engine();

    void generateEllipsoid(std::vector<float>& vertices, int slices, int stacks) {
    for (int i = 0; i <= stacks; ++i) {
        float theta = i * M_PI / stacks;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int j = 0; j <= slices; ++j) {
            float phi = j * 2 * M_PI / slices;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float x = cosPhi * sinTheta;
            float y = sinPhi * sinTheta;
            float z = cosTheta;

            float xr = WGS84::A * x;
            float yr = WGS84::A * y;
            float zr = WGS84::B * z;

            vertices.push_back(xr);
            vertices.push_back(yr);
            vertices.push_back(zr);
        }
    }
}

    void updateDeltaTime()
    {
        currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        previousTime = currentTime;
    }

    void initWindow();
    void initOpenGL();
    void mainLoop();
    void cleanup();
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


    GLFWwindow* window;

    ConsoleBuffer console;

    std::map<std::string, Shader> shaders;
    std::vector<FlyingObject> flyingObjects;
    std::vector<Camera> cameraViews;
    int currentCameraViewIndex=0;

    Earth& earth;


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
     {"rock", []COMMAND
     {
        std::ostringstream oss;
        try
        {
            Engine& engine = Engine::getInctance();
            Camera& currentCamera = engine.getCurrentCamera();
            float altitude = currentCamera.getLatitude();  // Latitude
            float longitude = currentCamera.getLongtitude(); // Longitude
            float height = currentCamera.getHeight();  // Height in meters
            glm::vec3 initialVelocity(10.0f, 0.0f, 0.0f); // Initial velocity
            glm::quat initialOrientation(glm::vec3(0.0f, 0.0f, 0.0f)); // No initial rotation
            Rock rock(engine,altitude, longitude, height, initialVelocity, initialOrientation);
            
        }
        catch(const std::exception& e)
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
     }}
     };
};

#endif // ENGINE_H

