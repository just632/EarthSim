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
    ~Camera() {}

    void update(){};
    void apply(){};
    void hangleUserInputs(){};
    void handleMouseMovement(){};


    float getLatitude()
    {
        return latitude;
    }

    float getLongtitude()
    {
        return longitude;
    }

    float getHeight()
    {
        return height;
    }


protected:
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 forward;

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
    GLFWwindow *window;
    

    
};

#endif // CAMERA_H