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
            timeMultiplier=1.f;
            instance = new Timer();
            return instance;
        }

        static float getTimeMultiplier(){
            return timeMultiplier;
        }

        static void setTimeMultiplier(float mult){
            timeMultiplier = mult;
        }

        float getDeltaTime()
        {
            return deltaTime;
        }

        float getDeltaSimTime()
        {
            return deltaSimTime;
        }

        float getTime()
        {
            return currentTime;
        }

        float getSimTime()
        {
            return simTime;
        }

        void updateDeltaTime()
        {
            currentTime = glfwGetTime();
            deltaTime = currentTime - previousTime;
            deltaSimTime = deltaTime * timeMultiplier;
            simTime+=deltaSimTime;
            previousTime = currentTime;
        }

    private:
        Timer() : deltaTime(0.0f), previousTime(glfwGetTime()), currentTime(0.0f) {}

        static Timer *instance;
        static float timeMultiplier;

        float deltaTime;
        float deltaSimTime;
        float previousTime;
        float currentTime; // application time
        float simTime; // simulation time
        
    };
    
}

