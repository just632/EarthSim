#pragma once
#include <Utils/wgs84.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <iostream> // Include any necessary headers here

class Engine;

class FlyingObject {
public:
    ~FlyingObject(){};

    void draw(){};
    void update(){};


protected:
    glm::vec3 position; // Derived from altitude, longitude, height
    glm::vec3 velocity;
    glm::quat orientation;
    float altitude;
    float longitude;
    float height;

    void updatePosition(){
        position = WGS84::toCartesian(longitude, altitude, height);
    };
};