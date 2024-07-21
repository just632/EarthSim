#ifndef ROCK_HPP
#define ROCK_HPP

#include "Objects/Object.hpp"

class Rock : public Object {
public:
    Rock(Engine* engine)
        : engine(engine), VAO(0), VBO(0), EBO(0) {
        setPosition(position);
        loadObject();
    }
    Rock(Engine* engine, float altitude, float longitude, float height, const glm::vec3& initialVelocity, const glm::quat& initialOrientation);

    void draw() override;
    void update() override {
    float deltaTime = engine->getDeltaTime();
    static const glm::vec3 gravity(0.0f, -9.81f, 0.0f);
    velocity += gravity * deltaTime;
    position += velocity * deltaTime;

    glm::vec3 latLonHeight = WGS84::toGeodetic(position);
    altitude = latLonHeight.x;
    longitude = latLonHeight.y;
    height = latLonHeight.z;
};

private:
    Engine* engine;
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void loadObject();
};

#endif // ROCK_HPP
