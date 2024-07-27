#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "Utils/Timer.hpp"
#include "Utils/Window.hpp"
#include "static/wgs84.hpp"

class Camera {
public:
    ~Camera() = default;

    virtual void update() = 0;
    virtual void handleUserInputs() = 0;  
    virtual void handleMouseMovement() = 0;

    float getLatitude() const
    {
        return latitude;
    }

    float getLongitude() const 
    {
        return longitude;
    }

    float getHeight() const
    {
        return height;
    }

    const glm::vec3& getPosition() const 
    {
        return position;
    }

    float getYaw() const 
    {
        return yaw;
    }

    float getPitch() const 
    {
        return pitch;
    }

    glm::vec3 getForward() const
    {
        return glm::normalize(glm::vec3(
        cos(pitch) * sin(yaw),
        sin(pitch),
        cos(pitch) * cos(yaw)
    ));
    }


    glm::mat4 getView(){return viewMatrix;};
    glm::mat4 getProjection(){return projectionMatrix;};

protected:
    Utils::Timer* timer = Utils::Timer::getInstance(); 
    Window* window = Window::getInstance();

    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 forward;
    glm::vec3 look;
    glm::vec3 WorldUp;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    float yaw;
    float pitch;
    float MouseLastX;
    float MouseLastY;
    float height;
    float longitude; 
    float latitude;
    float fieldOfView;
    float speed;
    float mouseSpeed;
};