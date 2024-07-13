#include "Rock.hpp"
#include "Engine.hpp"

Rock::Rock(Engine& engine):engine(engine){
    Camera& currentCamera = engine.getCurrentCamera();
    float altitude = currentCamera.getLatitude();  // Latitude
    float longitude = currentCamera.getLongtitude(); // Longitude
    float height = currentCamera.getHeight();  // Height in meters
    glm::vec3 initialVelocity(10.0f, 0.0f, 0.0f); // Initial velocity
    glm::quat initialOrientation(glm::vec3(0.0f, 0.0f, 0.0f)); // No initial rotation
    Rock(engine,altitude,longitude,height,initialVelocity,initialOrientation);
}

Rock::Rock(Engine& engine, float altitude, float longitude, float height, const glm::vec3& initialVelocity, const glm::quat& initialOrientation)
    : engine(engine) {
    loadObject();
}

void Rock::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Rock::update() {
    float deltaTime = engine.deltaTime;
    static const glm::vec3 gravity(0.0f, -9.81f, 0.0f);
    velocity += gravity * deltaTime;
    position += velocity * deltaTime;

    glm::vec3 latLonHeight = WGS84::toGeodetic(position);
    altitude = latLonHeight.x;
    longitude = latLonHeight.y;
    height = latLonHeight.z;
}

void Rock::loadObject() {
    vertices = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
    };

    indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 1, 5, 5, 4, 0,
        2, 3, 7, 7, 6, 2,
        0, 3, 7, 7, 4, 0,
        1, 2, 6, 6, 5, 1,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
