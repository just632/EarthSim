#pragma once
#include <static/wgs84.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>

#include "Utils/Shader.hpp"
#include "Utils/Timer.hpp"
#include "Cameras/Camera.hpp"

class Object
{
public:
    ~Object() = default;

    virtual void draw() = 0;   // method for drawing the object
    virtual void update() = 0; // method for updating the object

    // Setters for position, velocity, and orientation
    void setPosition(const glm::vec3 &pos)
    {
        position = pos;
        updateModelMatrix();
    }
    void setVelocity(const glm::vec3 &vel) { velocity = vel; }
    void setOrientation(const glm::quat &orient)
    {
        orientation = orient;
        updateModelMatrix();
    }

    // Getters for position, velocity, and orientation
    const glm::vec3 &getPosition() const { return position; }
    const glm::vec3 &getVelocity() const { return velocity; }
    const glm::quat &getOrientation() const { return orientation; }
    const glm::mat4 &getModelMatrix() const { return model; }

    std::shared_ptr<Shader> getShader(char *name);
    std::shared_ptr<Camera> getCurrentCamera();

    virtual void setShaderData() = 0;

protected:
    // Function to update the model matrix based on position and orientation
    void updateModelMatrix()
    {
        model = glm::mat4_cast(orientation); // Converts quaternion to rotation matrix
        model = glm::translate(model, position);
    }

    void checkGLError(const std::string &context)
    {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::cerr << "OpenGL error in " << context << ": " << err << std::endl;
        }
    }

    Utils::Timer *timer = Utils::Timer::getInstance();
    glm::vec3 position; // Derived from altitude, longitude, height
    glm::vec3 velocity;
    glm::quat orientation;
    glm::mat4 model;
    float altitude;
    float longitude;
    float height;
};
