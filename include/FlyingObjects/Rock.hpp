#ifndef ROCK_HPP
#define ROCK_HPP

#include "FlyingObjects/FlyingObject.hpp"

class Rock : public FlyingObject {
public:
    Rock(Engine& engine);
    Rock(Engine& engine, float altitude, float longitude, float height, const glm::vec3& initialVelocity, const glm::quat& initialOrientation);

    void draw();
    void update();

private:
    Engine& engine;
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void loadObject();
};

#endif // ROCK_HPP
