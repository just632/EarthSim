#pragma once
#include "Object.hpp"
#include <vector>
#include <GL/glew.h>

class Cube : public Object {
public:
    Cube( const glm::vec3& position)
        :  VAO(0), VBO(0), EBO(0) {
        setPosition(position);
        loadObject();

        
    }

    void draw() override {
        setShaderData();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void update() override {
        // For a static cube, we may not need to update anything.
        // If there's any logic to update, it goes here.

        model = glm::rotate(model,glm::radians(1.0f),glm::vec3(0,1,0));
    }

    void setShaderData() override {
    auto shader = getShader("simple");
    shader->use();
    auto cam = getCurrentCamera();
    shader->setMat4("model", model);
    shader->setMat4("view", cam->getView());
    shader->setMat4("projection", cam->getProjection());
    }

private:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void loadObject() {
        // Define the vertices and indices for a cube
        vertices = {
            // Positions          
            -0.5f, -0.5f, -0.5f,  // Bottom-left
             0.5f, -0.5f, -0.5f,  // Bottom-right
             0.5f,  0.5f, -0.5f,  // Top-right
            -0.5f,  0.5f, -0.5f,  // Top-left
            -0.5f, -0.5f,  0.5f,  // Bottom-left
             0.5f, -0.5f,  0.5f,  // Bottom-right
             0.5f,  0.5f,  0.5f,  // Top-right
            -0.5f,  0.5f,  0.5f   // Top-left
        };

        indices = {
            0, 1, 2, 2, 3, 0, // Front face
            4, 5, 6, 6, 7, 4, // Back face
            4, 5, 1, 1, 0, 4, // Bottom face
            7, 6, 2, 2, 3, 7, // Top face
            4, 0, 3, 3, 7, 4, // Left face
            1, 5, 6, 6, 2, 1  // Right face
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};
