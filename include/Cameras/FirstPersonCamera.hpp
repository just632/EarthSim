#pragma once
#include "Cameras/Camera.hpp"
#include "static/wgs84.hpp"

class FirstPersonCamera : public Camera {
public:
    FirstPersonCamera();
    FirstPersonCamera(glm::vec3 startPosition, float fov, float movementSpeed, float mouseSensitivity);

    void update() override;
    void handleUserInputs() override;
    void handleMouseMovement() override;
    void apply();
private:
    void computeMatrices();
    
    glm::mat4 modleMatrix = glm::mat4(1.0f);
};