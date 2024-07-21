#include <GL/glew.h>
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "controls.hpp"
#include "config.h"
#include <string.h>

// WGS84 Parameters
const double a = 63.783880f; // Semi-major axis in meters
const double b = 63.569120f; // Semi-minor axis in meters

const double f = 1.0f - (a / b); // Flattening
const double e2 = f * (2 - f); // First eccentricity squared
const double gamma_e = 9.7803253359; // Equatorial gravity in m/s^2
const double k = 0.00193185265241; // Gravity constant

// Initial position : on +Z
glm::vec3 position(0, b, 0);

glm::vec3 up;
glm::vec3 right;
glm::vec3 forward;
glm::vec3 WorldUp(0.f,1.f,0.f);

// Initial horizontal angle : toward -Z
float yaw = 3.14f;
// Initial vertical angle : none
float pitch = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.002f;



glm::mat4 viewMatrix;
glm::mat4 projectionMatrix = glm::perspective(glm::radians(initialFoV), 4.0f / 3.0f, 0.1f, 10000.0f);
glm::mat4 modleMatrix = glm::mat4(1.0f);
// Get the view matrix
glm::mat4 getViewMatrix() {
    return viewMatrix;
}

glm::mat4 getModleMatrix(){
    return modleMatrix;
}

// Get the projection matrix
glm::mat4 getProjectionMatrix() {
    return projectionMatrix;
}

// Callback for scroll input to adjust speed
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)speed += yoffset;

}

// Function to calculate the prime vertical radius of curvature
double N(double phi) {
    return a / std::sqrt(1.0 - e2 * std::sin(phi) * std::sin(phi));
}

// Function to convert geodetic coordinates to Cartesian coordinates
void geodeticToCartesian(double phi, double lambda, double h, double& x, double& y, double& z) {
    double N_phi = N(phi);
    x = (N_phi + h) * std::cos(phi) * std::cos(lambda);
    y = (N_phi + h) * std::cos(phi) * std::sin(lambda);
    z = ((1 - e2) * N_phi + h) * std::sin(phi);
}

// Function to calculate normal gravity at a given latitude
double normalGravity(double phi) {
    return gamma_e * (1 + k * std::sin(phi) * std::sin(phi));
}

// Function to compute the new orientation of the camera
void computeViewMatrix() {
    // Calculate the forward vector
    forward = glm::vec3(
        cos(pitch) * sin(yaw),
        sin(pitch),
        cos(pitch) * cos(yaw)
    );

    // Normalize the position to get the up vector
    up = glm::normalize(position);

    // Right vector: cross product of forward and up
    right = glm::normalize(glm::cross(forward, up));

	// Calculate the lookAt point
    glm::vec3 lookAt = position + forward;

    // Recalculate forward vector to be tangent to the ellipsoid's surface
    forward = glm::normalize(glm::cross(up, right));

    // Return the view matrix
    viewMatrix = glm::lookAt(position, lookAt, up);
}


// Function to update the camera orientation based on mouse input
void handleMouseMovement() {
    static double lastX = WINDOW_W / 2, lastY = WINDOW_H / 2;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    yaw += mouseSpeed * float(lastX - xpos);
    pitch += mouseSpeed * float(lastY - ypos);
    // Update last mouse position
    lastX = xpos;
    lastY = ypos;

/* 
    // Clamp vertical angle to avoid flipping
    verticalAngle = glm::clamp(verticalAngle, -glm::half_pi<float>(), glm::half_pi<float>()); */
}

// Function to update the camera position based on keyboard input
void handleUserInputs(float deltaTime) {

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



void controlsUpdate(){
    static double lastTime = glfwGetTime();

    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    handleMouseMovement();

    handleUserInputs(deltaTime);

    computeViewMatrix();
    lastTime = currentTime;
}

void controlsApply(Shader* shader){

        shader->setMat4("view", getViewMatrix());
        shader->setMat4("projection", getProjectionMatrix());
        shader->setMat4("model", getModleMatrix());
}