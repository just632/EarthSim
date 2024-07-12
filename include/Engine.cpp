#include "Engine.hpp"


Engine::Engine() : windowWidth(800), windowHeight(600) {
    initWindow();
    initOpenGL();
    earth = Earth::getInstance();
}

Engine::~Engine() {
    for (auto& shader : shaders) {
        delete shader.second;
    }
    cleanup();
}

void Engine::run() {
    mainLoop();
}

void Engine::initWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(windowWidth, windowHeight, "Earth Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

void Engine::initOpenGL() {
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);
}

void Engine::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update and draw all flying objects
        for (auto obj : flyingObjects) {
            obj->update();
            obj->draw();
        }

        // Update and apply camera views
        cameraViews[currentCameraViewIndex]->update();
        cameraViews[currentCameraViewIndex]->applyView();
        
        // Draw the earth
        earth->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    engine->windowWidth = width;
    engine->windowHeight = height;
}

void Engine::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

        switch (key) {
            case GLFW_KEY_1:
                engine->switchCameraView(0);
                break;
            case GLFW_KEY_2:
                engine->switchCameraView(1);
                break;
            // Add more cases for additional views as needed
            default:
                break;
        }
    }
}

void Engine::switchCameraView(int viewIndex) {
    if (viewIndex >= 0 && viewIndex < cameraViews.size()) {
        currentCameraViewIndex = viewIndex;
    }
}

void Engine::passInputToView(char input){
    cameraViews[currentCameraViewIndex]->handleInput(input);
}

void Engine::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
