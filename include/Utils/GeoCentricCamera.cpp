#include "Utils/GeoCentricCamera.hpp"
#include "Utils/wgs84.hpp"
#include <Engine.hpp>

GeoCentricCamera::GeoCentricCamera(Engine *engine) : 
    GeoCentricCamera(engine, WGS84::toGeodetic(glm::vec3(0.f, WGS84::B, 0.f)), 90.f, 2.5f, 2.5f)
{
}

/*
    start position is a vector of (longitude:deg,latitude:deg,hight:meters)
*/
GeoCentricCamera::GeoCentricCamera(Engine *engine, glm::vec3 startPosition, float fov, float movementSpeed, float mouseSensitivity)
    : engine(engine)
{
    height = startPosition[2];
    longitude = startPosition[0];
    latitude = startPosition[1];
    position = WGS84::toCartesian(longitude, latitude, height);
    fieldOfView = fov;
    speed = movementSpeed;
    mouseSpeed = mouseSensitivity;

    up = glm::vec3(0, 1, 0);
    right = glm::vec3(1, 0, 0);
    forward = glm::vec3(0, 0, -1);
    yaw = 0.0f;
    pitch = 0.0f;

    computeMatrices();
}

void GeoCentricCamera::handleMouseMovement()
{
    double xpos, ypos;
    glfwGetCursorPos(engine->getWindow(), &xpos, &ypos);
    yaw += float(MouseLastX - xpos) * mouseSpeed;
    pitch += float(MouseLastY - ypos) * mouseSpeed;
    MouseLastX = xpos;
    MouseLastY = ypos;

    // Constrain the pitch value
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

void GeoCentricCamera::handleUserInputs()
{
    handleMouseMovement();
    // Movement speed factor
    float moveStep = engine->deltaTime * speed;
    GLFWwindow *window = engine->getWindow();
    // Adjust longitude and latitude based on movement
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        latitude += moveStep * cos(glm::radians(pitch));
        longitude += moveStep * sin(glm::radians(yaw));
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        latitude -= moveStep * cos(glm::radians(pitch));
        longitude -= moveStep * sin(glm::radians(yaw));
    }

    // Strafe right/left relative to the camera's direction
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        longitude += moveStep * cos(glm::radians(yaw));
        latitude -= moveStep * sin(glm::radians(yaw));
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        longitude -= moveStep * cos(glm::radians(yaw));
        latitude += moveStep * sin(glm::radians(yaw));
    }

    // Move up/down relative to the ellipsoid's center
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        height += moveStep;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        height -= moveStep;
    }
}

void GeoCentricCamera::update()
{
    handleUserInputs();
    // Calculate the new position based on longitude and latitude
    position = WGS84::toCartesian(longitude, latitude, height);

    // Get the surface normal at the current location
    up = WGS84::surfaceNormal(longitude, latitude);

    // Calculate forward vector based on yaw and pitch
    forward = glm::normalize(glm::vec3(
        cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
        sin(glm::radians(pitch)),
        cos(glm::radians(pitch)) * sin(glm::radians(yaw))));

    // Calculate right vector as the cross product of forward and up
    right = glm::normalize(glm::cross(forward, up));

    // Calculate the view matrix
    viewMatrix = glm::lookAt(position, position + forward, up);
}

void GeoCentricCamera::apply()
{
    // Get the current shader program
    auto shader = engine->getShader("simple");

    // Use the shader program
    shader->use();
    shader->setMat4("view", viewMatrix);
    shader->setMat4("projection", projectionMatrix);
}

void GeoCentricCamera::computeMatrices()
{
    // Calculate the new position based on longitude and latitude
    position = WGS84::toCartesian(longitude, latitude, height);

    // Get the surface normal at the current location
    up = WGS84::surfaceNormal(longitude, latitude);

    // Calculate forward vector based on yaw and pitch
    forward = glm::normalize(glm::vec3(
        cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
        sin(glm::radians(pitch)),
        cos(glm::radians(pitch)) * sin(glm::radians(yaw))));

    // Calculate right vector as the cross product of forward and up
    right = glm::normalize(glm::cross(forward, up));

    // Calculate the view matrix
    viewMatrix = glm::lookAt(position, position + forward, up);
    // Calculate the projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), engine->WindoAspectRatio, 0.1f, 100000.0f);
}
