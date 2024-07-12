#include "FirstPersonCamera.hpp"

/*
    start position is a vector of (longitude:deg,latitude:deg,hight:meters)
*/
FirstPersonCamera::FirstPersonCamera(glm::vec3 startPosition, float fov, float movementSpeed, float mouseSensitivity)
    : height(startPosition[2]) {
    position = startPosition;
    longitude = startPosition[0];
    latitude = startPosition[1];
    fieldOfView = fov;
    speed = movementSpeed;
    mouseSpeed = mouseSensitivity;

    up = glm::vec3(0, 1, 0);
    right = glm::vec3(1, 0, 0);
    backward = glm::vec3(0, 0, 1);

    computeMatrices();
}

void FirstPersonCamera::handleKeyPress(char key) {

    // Move forward/backward relative to the camera's direction
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += forward * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= forward * deltaTime * speed;
    }

    // Strafe right/left relative to the camera's direction
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * deltaTime * speed;
    }

    // Move up/down relative to the ellipsoid's center
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position += up * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        position -= up * deltaTime * speed;
    }
}

void FirstPersonCamera::update() {
    // Update camera based on user input
    // For simplicity, we'll assume input handling is done elsewhere and we just get the updated longitude, latitude, and position
    computeMatrices();
}

void FirstPersonCamera::apply() {
    // Apply the view and projection matrices
    // This can be done by setting the uniforms in the shaders
}

void FirstPersonCamera::computeMatrices() {
    // Calculate the new position based on longitude and latitude
    float radLong = glm::radians(longitude);
    float radLat = glm::radians(latitude);

    position.x = cos(radLat) * cos(radLong);
    position.z = cos(radLat) * sin(radLong);

    backward = glm::normalize(position);
    right = glm::normalize(glm::cross(up, backward));
    up = glm::normalize(glm::cross(backward, right));

    // Calculate the view matrix
    viewMatrix = glm::lookAt(position, position + backward, up);

    // Calculate the projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), 4.0f / 3.0f, 0.1f, 100.0f);
}
