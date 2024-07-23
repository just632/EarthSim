#pragma once
#include "Object.hpp"
#include <vector>
#include <GL/glew.h>

class Triangle : public Object {
public:
    Triangle()
        :  VAO(0), VBO(0), EBO(0) {
        loadObject();

        
    }

    void draw() override {
        setShaderData();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES,0,3);
        glBindVertexArray(0);
    }

    void update() override {
        // For a static cube, we may not need to update anything.
        // If there's any logic to update, it goes here.
    }

    void setShaderData() override {
    auto shader = getShader("simple_triangle");
    shader->use();
    }

private:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void loadObject() {
        // Define the vertices and indices for a cube
    vertices = {
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top 
    };

    indices = {  // note that we start from 0!
        0, 1, 3,   
    };  

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	// Vertex attributes stay the same
    glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};
