#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
namespace Utils
{
    class Timer
    {
    public:
        Timer(const Timer &obj) = delete;
        static Timer *getInstance()
        {
            if (instance != nullptr)
            {
                return instance;
            }
            instance = new Timer();
            return instance;
        }

        float getDeltaTime()
        {
            return deltaTime;
        }

        float getTime()
        {
            return currentTime;
        }

        void updateDeltaTime()
        {
            currentTime = glfwGetTime();
            deltaTime = currentTime - previousTime;
            previousTime = currentTime;
        }

    private:
        Timer() : deltaTime(0.0f), previousTime(glfwGetTime()), currentTime(0.0f) {}

        static Timer *instance;

        float deltaTime;
        float previousTime;
        float currentTime;

        
    };
}
