#ifndef GEOCENTRICCAMERA_H
#define GEOCENTRICCAMERA_H

#include "Utils/Camera.hpp"

class GeoCentricCamera : public Camera {
public:
    GeoCentricCamera(Engine* engine);
    GeoCentricCamera(Engine* engine,glm::vec3 startPosition, float fov, float movementSpeed, float mouseSensitivity);

    void update() override;
    void apply() override;
    void handleUserInputs() override;
    void handleMouseMovement() override;

private:
    void computeMatrices();


        // UNSAFE AS F*CK
    Engine *engine; // DO NOT MODIFY!!! , ONLY USE AS A READ REFRENCE!!!
};

#endif // GEOCENTRICCAMERA_H
