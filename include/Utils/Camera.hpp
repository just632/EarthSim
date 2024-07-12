#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class Camera {
public:
    virtual ~Camera() {}

    virtual void update() = 0;
    virtual void apply() = 0;
    virtual void handleKeyPress(char key) = 0;

protected:
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 backward;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    float longitude;
    float latitude;
    float fieldOfView;
    float speed;
    float mouseSpeed;

    
};

#endif // CAMERA_H
