#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H

#include "Utils/Camera.hpp"

class FirstPersonCamera : public Camera {
public:
    FirstPersonCamera(Engine* engine);
    FirstPersonCamera(Engine* engine,glm::vec3 startPosition, float fov, float movementSpeed, float mouseSensitivity);

    void update() override;
    void apply() override;
    void handleUserInputs() override;
    void handleMouseMovement() override;

private:
    void computeMatrices();

    glm::mat4 modleMatrix = glm::mat4(1.0f);
        // UNSAFE AS F*CK
    Engine *engine; // DO NOT MODIFY!!! , ONLY USE AS A READ REFRENCE!!!
    GLFWwindow* window;
};

#endif // FIRSTPERSONCAMERA_H
