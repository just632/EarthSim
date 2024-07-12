#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H

#include "Camera.hpp"

class FirstPersonCamera : public Camera {
public:
    FirstPersonCamera(glm::vec3 startPosition, float fov, float movementSpeed, float mouseSensitivity);

    void update() override;
    void apply() override;
    void handleKeyPress(char key) override;

private:
    void computeMatrices();

    float height;
};

#endif // FIRSTPERSONCAMERA_H
