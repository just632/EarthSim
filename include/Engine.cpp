#include "Engine.hpp"
#include "Utils/FirstPersonCamera.hpp"
//#include "Utils/GeoCentricCamera.hpp"




Engine::Engine() : windowWidth(800.f), windowHight(600.f) /*earth(Earth::getInstance()),*/
{
    std::cout<<"Engine Created"<<std::endl;
    
    WindoAspectRatio= windowWidth / windowHight;
    initWindow();
    initOpenGL();

    console=std::make_shared<ConsoleBuffer>(this,10);
    std::cout << this << " Engine::Engine this" << std::endl;
    //cameraViews.push_back(std::make_shared<GeoCentricCamera>(*this));
    cameraViews.push_back(std::make_shared<FirstPersonCamera>(this));

    Objects.push_back(std::make_shared<Ellipsoid>(this,glm::vec3(1.f),500,100));
    //earth.load();
    console->load();
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

void Engine::initWindow()
{
    std::cout<<"Engine::initWindow"<<std::endl;
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout<<"Engine::initWindow::glfwInit Done"<<std::endl;
    window = glfwCreateWindow(windowWidth, windowHight, "Earth Simulation", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    std::cout<<"Engine::window = "<<window<<std::endl;
    std::cout<<"Engine::initWindow::glfwCreateWindow Done"<<std::endl;
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // Set viewport and callbacks
    glViewport(0, 0, windowWidth, windowHight);

    std::cout<<"aspect : "<<WindoAspectRatio<<std::endl;
    // Dark blue background
    glClearColor(0.1f, 0.1f, 0.4f, 0.0f);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window,keyCallback);

}

void Engine::initOpenGL()
{
    glewExperimental = GL_TRUE; 
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void Engine::mainLoop()
{
    console->debugInfo=true;

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {

        updateDeltaTime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




        drawCamera();

        drawObjects();

        //earth.render();

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
}

void Engine::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    engine->windowWidth = width;
    engine->windowHight = height;
    engine->WindoAspectRatio = width / height;
}

#define INPUT_SHIFT key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT
#define INPUT_SHIFT_DOWN action == GLFW_PRESS && (INPUT_SHIFT)
#define INPUT_SHIFT_UP action == GLFW_RELEASE && (INPUT_SHIFT)
#define INPUT_NOT_SHIFT action == GLFW_PRESS && !(INPUT_SHIFT)

void Engine::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    static bool shift_down = false;
    Engine *engine = static_cast<Engine *>(glfwGetWindowUserPointer(window));
    auto console = engine->console;

    if (action == GLFW_PRESS)
    {

        switch (key)
        {
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
    
    if(INPUT_SHIFT_DOWN)shift_down=true;
    if(INPUT_SHIFT_UP)shift_down=false;
    if(INPUT_NOT_SHIFT)console->addInput(key,shift_down);
}

void Engine::switchCameraView(int viewIndex)
{
    if (viewIndex >= 0 && viewIndex < cameraViews.size())
    {
        currentCameraViewIndex = viewIndex;
    }
}

void Engine::passInputToView(char input)
{
}

void Engine::cleanup()
{
    glfwDestroyWindow(window);
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



