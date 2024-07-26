#pragma once
#include "Objects/Object.hpp"

class Rock : public Object {
public:
    Rock()
    {
        orientation = glm::quat(1.0, 0.0, 0.0, 0.0);
        loadObject();
        setForces();
    }

    Rock(const glm::vec3& position, const glm::vec3& initialVelocity, const glm::quat& initialOrientation)
    {
        setPosition(position);
        setVelocity(initialVelocity);
        setOrientation(initialOrientation);
        loadObject();
        setForces();
        updateModelMatrix();
    }

    void draw() override {
        // Bind VAO and draw the rock object
        setShaderData();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void setShaderData() override {
        auto shader = getShader("phisical");
        shader->use();
        auto cam = getCurrentCamera();
        shader->setMat4("model", model);
        shader->setMat4("view", cam->getView());
        shader->setMat4("projection", cam->getProjection());
        shader->setVec3("objectColor", glm::vec3(0.3f, 0.1f, 0.1f)); // Example color
        shader->setVec3("lightColor", glm::vec3(1.0f));
        shader->setVec3("lightPos", glm::vec3(100.f,500.f,100.f));
        shader->setVec3("viewPos", cam->getPosition());
    }

    void update() override {
        double deltaTime = timer->getDeltaSimTime();
        calculateAndApplyForces(deltaTime);
    }

private:
    void setForces(){
        addForce(std::make_unique<GravityForce>(mass));
        //addForce(std::make_unique<DragForce>(mass, 0.47));
    }

    void loadObject() {
        // Define the vertices and indices for the rock
        vertices = {
            // positions          // normals       
            // front face
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
             0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  

            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 
             0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 
             0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 
            -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,         
        };

        indices = {
            // front face
            0, 1, 2,
            2, 3, 0,
            // back face
            4, 5, 6,
            6, 7, 4,
            // left face
            0, 4, 7,
            7, 3, 0,
            // right face
            1, 5, 6,
            6, 2, 1,
            // top face
            3, 2, 6,
            6, 7, 3,
            // bottom face
            0, 1, 5,
            5, 4, 0
        };

        // Generate and bind VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Generate and bind VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Generate and bind EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Unbind VAO
        glBindVertexArray(0);
    }
};
