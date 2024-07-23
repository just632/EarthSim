// Include GLEW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <common/config.h>
#include <Utils/Shader.hpp>
#include <Utils/Console.hpp>
#include <Utils/Window.hpp>
#include <Utils/Timer.hpp>
#include <Engine.hpp>
#include <Objects/Ellipsoid.hpp>

int main() {
    auto engine = Engine::getInstance();
    engine->run();
    // auto console = Console::getInstance();
    // auto window = Window::getInstance();
    // auto timer = Utils::Timer::getInstance();

    // // Create and compile shaders
    
    // // Main loop
    // while (!window->keyPressed(GLFW_KEY_ESCAPE) && !glfwWindowShouldClose(window->getWindow_ptr())) {

    //     timer->updateDeltaTime();
    //     // Render
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //     engine->updateCamera();
    //     engine->drawObjects();
    //     engine->drawConsole();

    //     glfwSwapBuffers(window->getWindow_ptr());
    //     glfwPollEvents();
    // }

    // // Cleanup

    // // Terminate GLFW
    // glfwTerminate();
    return 0;
}
