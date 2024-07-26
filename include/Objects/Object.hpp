#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "Utils/Shader.hpp"
#include "Utils/Timer.hpp"
#include "Cameras/Camera.hpp"
#include "Utils/Physics.hpp"

class Object {
public:
    virtual ~Object() = default;

    virtual void draw() = 0;   // method for drawing the object
    virtual void update() = 0; // method for updating the object
    virtual void setShaderData() = 0;



protected:

    GLuint VAO, VBO, EBO = 0;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    std::shared_ptr<Shader> getShader(char *name);
    std::shared_ptr<Camera> getCurrentCamera();


    // Function to update the model matrix based on position and orientation
    void updateModelMatrix() {
        model = glm::mat4_cast(orientation); // Converts quaternion to rotation matrix
        model = glm::translate(model, glm::vec3(position.getECEF()));
    }

    // Function to calculate and apply all forces
    void calculateAndApplyForces(double deltaTime) {
        if(position.getAltitude()<=0){
            position.setVelocity(glm::vec3(0.f));
            updateModelMatrix();
            return;
        }
        for (const auto &force : forces) {
            force->apply(position, deltaTime);
        }
        position.calculateAndApplyForces(mass, deltaTime);
        updateModelMatrix();
    }

    void checkGLError(const std::string &context) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error in " << context << ": " << err << std::endl;
        }
    }

        // Setters for position in ECEF courdinets 
    void setPosition(const glm::vec3 &pos) {
        position = Position(pos);
    }

    void setVelocity(const glm::vec3 &vel) {
        position.setVelocity(vel);
    }

    void setOrientation(const glm::quat &orient) {
        orientation = orient;
    }

    void setMass(double newMass) {
        mass = newMass;
    }

    // Getters for position, velocity, and orientation
    const glm::vec3 getPosition() const {
        return position.getECEF();
    }

    const glm::vec3 getVelocity() const {
        return position.getVelocity();
    }

    const glm::quat &getOrientation() const { return orientation; }
    const glm::mat4 &getModelMatrix() const { return model; }


    // Method to add a force to the object
    void addForce(std::unique_ptr<Force> force) {
        forces.push_back(std::move(force));
    }

    Utils::Timer *timer = Utils::Timer::getInstance();
    Position position{0.0, 0.0, 0.0}; // Initialized with default coordinates
    glm::quat orientation;
    glm::mat4 model;
    double mass = 1.0f; // Assuming a default mass, can be set differently if needed
    std::vector<std::unique_ptr<Force>> forces;
};
