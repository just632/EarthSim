#include "Cameras/FirstPersonCamera.hpp"


FirstPersonCamera::FirstPersonCamera()
    : FirstPersonCamera(glm::vec3(0.f, 63.569120f, 0.f), 45.f, 2.5f, 0.002f)
{
}

FirstPersonCamera::FirstPersonCamera(glm::vec3 startPosition, float fov, float movementSpeed, float mouseSensitivity)
{
    position = startPosition;
    fieldOfView = fov;
    speed = movementSpeed;
    mouseSpeed = mouseSensitivity;

    WorldUp = glm::vec3(0, 1, 0);
    right = glm::vec3(1, 0, 0);
    forward = glm::vec3(0, 0, -1);
    yaw = 3.14f;
    pitch = 0.0f;
    double xpos, ypos;
    glfwGetCursorPos(window->getWindow_ptr(), &xpos, &ypos);

    MouseLastX = xpos;
    MouseLastY = ypos;
    computeMatrices();
}

void FirstPersonCamera::handleMouseMovement() {
    double xpos, ypos;
    glfwGetCursorPos(window->getWindow_ptr(), &xpos, &ypos);

    yaw += mouseSpeed * float(MouseLastX - xpos);
    pitch -= mouseSpeed * float(MouseLastY - ypos);
    // Update last mouse position
    
    MouseLastX = xpos;
    MouseLastY = ypos;

    // Constrain the pitch value
    // if (pitch > 89.0f) pitch = 89.0f;
    // if (pitch < -89.0f) pitch = -89.0f;
}

void FirstPersonCamera::handleUserInputs() {
    handleMouseMovement();
    float moveStep = timer->getDeltaTime() * speed;
    if (window->keyPressed( GLFW_KEY_UP) || window->keyPressed( GLFW_KEY_W)) {
        position += forward * moveStep;
    }
    if (window->keyPressed( GLFW_KEY_DOWN) || window->keyPressed( GLFW_KEY_S)) {
        position -= forward * moveStep;
    }
    if (window->keyPressed( GLFW_KEY_RIGHT) || window->keyPressed( GLFW_KEY_D)) {
        position += right * moveStep;
    }
    if (window->keyPressed( GLFW_KEY_LEFT) || window->keyPressed( GLFW_KEY_A)) {
        position -= right * moveStep;
    }
    if (window->keyPressed( GLFW_KEY_SPACE)) {
        position += WorldUp * moveStep;
    }
    if (window->keyPressed( GLFW_KEY_LEFT_CONTROL)) {
        position -= WorldUp * moveStep;
    }
}

void FirstPersonCamera::update() {
    handleUserInputs();
    computeMatrices();
}



void FirstPersonCamera::computeMatrices() {
    forward = glm::normalize(glm::vec3(
        cos(pitch) * sin(yaw),
        sin(pitch),
        cos(pitch) * cos(yaw)
    ));
    up = glm::normalize(position);
     // Right vector: cross product of forward and world up
    right = glm::normalize(glm::cross(forward, up));

    glm::vec3 lookAt = position + forward;

    // Recalculate the up vector
    forward = glm::normalize(glm::cross(up, right));

    // Compute the view matrix
    viewMatrix = glm::lookAt(position, lookAt, up);
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), window->getAspectRatio(), 0.1f, 100.0f);
}