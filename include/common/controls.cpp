#include <GL/glew.h>
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

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

// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.002f;



glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(initialFoV), 4.0f / 3.0f, 0.1f, 10000.0f);
glm::mat4 ModleMatrix = glm::mat4(1.0f);
// Get the view matrix
glm::mat4 getViewMatrix() {
    return ViewMatrix;
}

glm::mat4 getModleMatrix(){
    return ModleMatrix;
}

// Get the projection matrix
glm::mat4 getProjectionMatrix() {
    return ProjectionMatrix;
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
glm::mat4 computeViewMatrix(const glm::vec3& position, float horizontalAngle, float verticalAngle) {
    // Calculate the forward vector
    forward = glm::vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
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
    return glm::lookAt(position, lookAt, up);
}

// Function to update the camera orientation based on mouse input
void updateOrientation(float deltaX, float deltaY) {
    horizontalAngle += mouseSpeed * deltaX;
    verticalAngle += mouseSpeed * deltaY;
/* 
    // Clamp vertical angle to avoid flipping
    verticalAngle = glm::clamp(verticalAngle, -glm::half_pi<float>(), glm::half_pi<float>()); */
}

// Function to update the camera position based on keyboard input
void updatePosition(float deltaTime) {


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

// Main function to compute matrices from inputs
void computeMatricesFromInputs() {
    // Static variables for time and mouse position
    static double lastTime = glfwGetTime();
    static double lastX = WINDOW_W / 2, lastY = WINDOW_H / 2;

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Calculate mouse delta
    float deltaX = float(lastX - xpos);
    float deltaY = float(lastY - ypos); // Reversed since y-coordinates go from bottom to top

    // Update last mouse position
    lastX = xpos;
    lastY = ypos;

    // Update orientation and position
    updateOrientation(deltaX, deltaY);
    updatePosition(deltaTime);

    // Update view matrix
    ViewMatrix = computeViewMatrix(position, horizontalAngle, verticalAngle);

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
