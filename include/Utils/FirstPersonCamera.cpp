#include "Utils/FirstPersonCamera.hpp"
#include "Utils/wgs84.hpp"
#include <Engine.hpp>


FirstPersonCamera::FirstPersonCamera(Engine& engine):engine(engine){
    glm::vec3 startPosition(0.f,0.f,WGS84::A+1000.f);
    float fov;
    float movementSpeed;
    float mouseSensitivity;
    FirstPersonCamera(engine,startPosition,fov,movementSpeed,mouseSensitivity);
}

/*
    start position is a vector of (longitude:deg,latitude:deg,hight:meters)
*/
FirstPersonCamera::FirstPersonCamera(Engine& engine,glm::vec3 startPosition, float fov, float movementSpeed, float mouseSensitivity)
    : engine(engine)
    {
    window = engine.getWindow();    
    height = startPosition[2] ;
    longitude = startPosition[0];
    latitude = startPosition[1];
    position = WGS84::toCartesian(longitude,latitude,height);
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

void FirstPersonCamera::handleMouseMovement() {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    yaw += float(MouseLastX - xpos) * mouseSpeed;
    pitch += float(MouseLastY - ypos) * mouseSpeed;
    MouseLastX = xpos;
    MouseLastY = ypos;

    // Constrain the pitch value
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void FirstPersonCamera::hangleUserInputs() {
    // Movement speed factor
    float moveStep = engine.deltaTime * speed;

    // Calculate forward vector based on yaw and pitch
    forward = glm::normalize(glm::vec3(
        cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
        sin(glm::radians(pitch)),
        cos(glm::radians(pitch)) * sin(glm::radians(yaw))
    ));

    // Calculate right vector as the cross product of forward and up
    right = glm::normalize(glm::cross(forward, up));

    // Adjust longitude and latitude based on movement
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        latitude += moveStep * cos(glm::radians(pitch));
        longitude += moveStep * sin(glm::radians(yaw));
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        latitude -= moveStep * cos(glm::radians(pitch));
        longitude -= moveStep * sin(glm::radians(yaw));
    }

    // Strafe right/left relative to the camera's direction
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        longitude += moveStep * cos(glm::radians(yaw));
        latitude -= moveStep * sin(glm::radians(yaw));
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        longitude -= moveStep * cos(glm::radians(yaw));
        latitude += moveStep * sin(glm::radians(yaw));
    }

    // Move up/down relative to the ellipsoid's center
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        height += moveStep;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        height -= moveStep;
    }

    // Update the position based on the new longitude, latitude, and height
    position = WGS84::toCartesian(longitude, latitude, height);
}

void FirstPersonCamera::update() {
    computeMatrices();
}

void FirstPersonCamera::apply() {
    // Get the current shader program
    Shader shader = engine.getShader("simple");

    // Use the shader program
    shader.use();

    // Set the view matrix
    GLint viewMatrixLocation = shader.getUniformLocation("viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    // Set the projection matrix
    GLint projectionMatrixLocation = shader.getUniformLocation("projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void FirstPersonCamera::computeMatrices() {
    // Calculate the new position based on longitude and latitude
    position = WGS84::toCartesian(longitude, latitude, height);

    // Get the surface normal at the current location
    up = WGS84::surfaceNormal(longitude, latitude);

    // Calculate forward vector based on yaw and pitch
    forward = glm::normalize(glm::vec3(
        cos(glm::radians(pitch)) * cos(glm::radians(yaw)),
        sin(glm::radians(pitch)),
        cos(glm::radians(pitch)) * sin(glm::radians(yaw))
    ));

    // Calculate right vector as the cross product of forward and up
    right = glm::normalize(glm::cross(forward, up));

    // Calculate the view matrix
    viewMatrix = glm::lookAt(position, position + forward, up);
    // Calculate the projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView),engine.WindoAspectRatio, 0.1f, 100000.0f);
}
