#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H

#include "Utils/Camera.hpp"

class FirstPersonCamera : public Camera {
public:
    FirstPersonCamera(Engine& engine);
    FirstPersonCamera(Engine& engine,glm::vec3 startPosition, float fov, float movementSpeed, float mouseSensitivity);

    void update();
    void apply();
    void hangleUserInputs();
    void handleMouseMovement();

private:
    void computeMatrices();


        // UNSAFE AS F*CK
    Engine &engine; // DO NOT MODIFY!!! , ONLY USE AS A READ REFRENCE!!!
};

#endif // FIRSTPERSONCAMERA_H
