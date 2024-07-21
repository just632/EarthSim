#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class Engine;

class Camera {
public:
    virtual ~Camera() = default;  // Virtual destructor

    virtual void update() = 0;
    virtual void apply() = 0;
    virtual void handleUserInputs() = 0;  // Corrected typo
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

protected:
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 forward;
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

#endif // CAMERA_H
