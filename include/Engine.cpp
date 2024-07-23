#include "Engine.hpp"
#include "Cameras/FirstPersonCamera.hpp"
//#include "Utils/GeoCentricCamera.hpp"



Engine *Engine::instance = nullptr;

Engine::Engine() /*earth(Earth::getInstance()),*/
{
    std::cout<<"Engine Created"<<std::endl;

    console->init();
    console->debugInfo=true;
    loadCommands();
    cameraViews.push_back(std::make_shared<FirstPersonCamera>());
    Objects.push_back(std::make_shared<Ellipsoid>(glm::vec3(0.f),100,100));
    setGlfwCallbacks();
}

Engine::~Engine()
{
    cleanup();
}

void Engine::run()
{
    try
    {
        mainLoop();
    }
    catch (const std::exception &e)
    {
        std::cout << "A RunTime Exeption has occured " << std::endl;
        std::cerr << e.what() << std::endl;
        std::cout << "PRESS ENTER TO CLOSE" << std::endl;
        std::cin.get();
    }
}

void Engine::mainLoop()
{
    while (!glfwWindowShouldClose(window->getWindow_ptr()))
    {

        timer->updateDeltaTime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




        updateCamera();

        drawObjects();
        drawConsole();
        //earth.render();

        glfwSwapBuffers(window->getWindow_ptr());
        glfwPollEvents();

    }
}

void Engine::framebufferSizeCallback(GLFWwindow *window_ptr, int width, int height)
            {
                glViewport(0, 0, width, height);
                Window* window = Window::getInstance();
                window->setWindowSize( width,height);
            }

#define INPUT_SHIFT key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT
#define INPUT_SHIFT_DOWN action == GLFW_PRESS && (INPUT_SHIFT)
#define INPUT_SHIFT_UP action == GLFW_RELEASE && (INPUT_SHIFT)
#define INPUT_NOT_SHIFT action == GLFW_PRESS && !(INPUT_SHIFT)

void Engine::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    static bool shift_down = false;
    auto console = Console::getInstance();
    
    if(INPUT_SHIFT_DOWN)shift_down=true;
    if(INPUT_SHIFT_UP)shift_down=false;
    if(INPUT_NOT_SHIFT)console->handleChar(key,shift_down);
}

void Engine::switchCameraView(int viewIndex)
{
    if (viewIndex >= 0 && viewIndex < cameraViews.size())
    {
        currentCameraViewIndex = viewIndex;
    }
}

void Engine::cleanup()
{
    glfwDestroyWindow(window->getWindow_ptr());
    glfwTerminate();
}

std::shared_ptr<Shader> Engine::getShader(char* name)
{
    // Check if the shader is already loaded
    auto it = shaders.find(name);
    if (it != shaders.end())
    {
        return it->second;
    }

    // Create and load the shader
    std::shared_ptr<Shader> shader = std::make_shared<Shader>(name, name);
    shader->load();
    // Store the shader in the map and return it
    shaders[name] = shader;
    return shader;
}



