#ifndef ELLIPSOID_HPP
#define ELLIPSOID_HPP

#include "Object.hpp"
#include <vector>
#include <GL/glew.h>
#include<Utils/wgs84.hpp>

class Ellipsoid : public Object {
public:
    Ellipsoid(Engine* engine, const glm::vec3& position,int stacks,int slices)
        : engine(engine), VAO(0), VBO(0), EBO(0) {
        setPosition(position);
        loadObject(stacks,slices);
    }

    void draw() override {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);
        glBindVertexArray(0);
    }

    void update() override {
        // For a static cube, we may not need to update anything.
        // If there's any logic to update, it goes here.
    }

private:
    Engine* engine;
    GLuint VAO, VBO, EBO;
    GLsizei size;
    std::vector<float> vertices;

    void loadObject(int stacks,int slices) {
        // Generate ellipsoid vertices

    for (int i = 0; i <= stacks; ++i) {
        float theta = i * M_PI / stacks;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int j = 0; j <= slices; ++j) {
            float phi = j * 2 * M_PI / slices;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float x = cosPhi * sinTheta;
            float y = sinPhi * sinTheta;
            float z = cosTheta;

            float xr = WGS84::A * x;
            float yr = WGS84::A * y;
            float zr = WGS84::B * z;

            vertices.push_back(xr);
            vertices.push_back(yr);
            vertices.push_back(zr);
        }
    }


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    }
};

#endif // ELLIPSOID_HPP
