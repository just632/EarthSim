#include "Utils/FirstPersonCamera.hpp"
#include "Utils/wgs84.hpp"
#include <Engine.hpp>


FirstPersonCamera::FirstPersonCamera(Engine* engine)
    : FirstPersonCamera(engine, glm::vec3(0.f, 63.569120f, 0.f), 45.f, 2.5f, 0.002f)
{
}

FirstPersonCamera::FirstPersonCamera(Engine* engine, glm::vec3 startPosition, float fov, float movementSpeed, float mouseSensitivity)
    : engine(engine)
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
    window = engine->getWindow();
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    MouseLastX = xpos;
    MouseLastY = ypos;
    computeMatrices();
}

void FirstPersonCamera::handleMouseMovement() {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

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
    float moveStep = engine->getDeltaTime() * speed;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += forward * moveStep;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= forward * moveStep;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * moveStep;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * moveStep;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position += WorldUp * moveStep;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        position -= WorldUp * moveStep;
    }
}

void FirstPersonCamera::update() {
    handleUserInputs();
    computeMatrices();
}

void FirstPersonCamera::apply() {
    auto shader = engine->getShader("simple");

    shader->setMat4("model", modleMatrix);
    shader->setMat4("view", viewMatrix);
    shader->setMat4("projection", projectionMatrix);
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
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), engine->WindoAspectRatio, 0.1f, 100.0f);
}