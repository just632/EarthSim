#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Window
{

public:
        Window(const Window &obj) = delete;
        static Window *getInstance()
        {
            if (instance != nullptr)
            {
                return instance;
            }
            instance = new Window();
            return instance;
        }
    ~Window() = default;

    GLFWwindow *getWindow_ptr()
    {
        return window_ptr;
    }

    void init()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window_ptr = glfwCreateWindow(windowWidth, windowHight, "Earth Simulation", nullptr, nullptr);
        if (!window_ptr)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(window_ptr);
        glfwSetInputMode(window_ptr, GLFW_STICKY_KEYS, GL_TRUE);
        glfwSetInputMode(window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glViewport(0, 0, windowWidth, windowHight);

        // Dark blue background
        glClearColor(0.1f, 0.1f, 0.4f, 0.0f);

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            exit(EXIT_FAILURE);
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void SetWindowUserPointer(void *pointer)
    {
        glfwSetWindowUserPointer(window_ptr, pointer);
    }

    bool keyPressed(int key)
    {
        return glfwGetKey(window_ptr, key) == GLFW_PRESS;
    }

    float getWidth() { return windowWidth; }
    float getHight() { return windowHight; }
    float getAspectRatio() { return windowAspectRatio; }

    void setWindowSize(float width, float hight)
    {
        windowWidth = width;
        windowHight = hight;
        windowAspectRatio = width / hight;
    }

private:
    static Window *instance;

    Window() : windowWidth(800.f), windowHight(600.f)
    {
        setWindowSize(windowWidth,windowHight);
        init();
    }

    GLFWwindow *window_ptr;

    float windowAspectRatio;
    float windowWidth;
    float windowHight;
};