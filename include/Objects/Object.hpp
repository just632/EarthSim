#pragma once
#include <Utils/wgs84.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream> 

class Engine;

class Object {
public:
     ~Object() = default;

     virtual void draw()=0; // method for drawing the object
     virtual void update()=0; // method for updating the object

    // Setters for position, velocity, and orientation
    void setPosition(const glm::vec3& pos) { position = pos; updateModelMatrix(); }
    void setVelocity(const glm::vec3& vel) { velocity = vel; }
    void setOrientation(const glm::quat& orient) { orientation = orient; updateModelMatrix(); }

    // Getters for position, velocity, and orientation
    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getVelocity() const { return velocity; }
    const glm::quat& getOrientation() const { return orientation; }
    const glm::mat4& getModelMatrix() const { return model; }

protected:
    // Function to update the model matrix based on position and orientation
    void updateModelMatrix() {
        model = glm::mat4_cast(orientation); // Converts quaternion to rotation matrix
        model = glm::translate(model, position);
    }

    glm::vec3 position; // Derived from altitude, longitude, height
    glm::vec3 velocity;
    glm::quat orientation;
    glm::mat4 model;
    float altitude;
    float longitude;
    float height;
};

