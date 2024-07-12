#ifndef ENGINE_H
#define ENGINE_H

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Utils/Shader.hpp"
#include "Utils/Camera.hpp"

#include "Earth.hpp"
#include "FlyingObjects/FlyingObject.hpp"

class Engine {
public:
    Engine();
    ~Engine();
    void run();
    void switchCameraView(int viewIndex);
    void passInputToView(char input);

private:
    void initWindow();
    void initOpenGL();
    void mainLoop();
    void cleanup();
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


    GLFWwindow* window;
    int windowWidth;
    int windowHeight;

    bool consoleOpen;

    std::map<std::string, Shader*> shaders;
    std::vector<FlyingObject*> flyingObjects;
    std::vector<CameraView*> cameraViews;
    int currentCameraViewIndex;

    Earth* earth;
};

#endif // ENGINE_H
