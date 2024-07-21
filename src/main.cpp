// Include GLEW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <common/config.h>
#include <Utils/Shader.hpp>
#include <Engine.hpp>
#include <Objects/Ellipsoid.hpp>

GLFWwindow* window;
const float R0 = 63.783880f; // Equatorial radius
const float Rp = 63.569120f; // Polar radius
const float f = 1.0f - (Rp / R0); // Flattening


void generateEllipsoid(GLuint& VAO,GLuint& VBO,GLsizei& size, int slices, int stacks) {
        // Generate ellipsoid vertices
    std::vector<float> vertices;

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

            float xr = R0 * x;
            float yr = R0 * y;
            float zr = Rp * z;

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

    size = vertices.size();
}


int main() {
    auto engine = Engine::getInctance();
    auto console = engine.getConsole();
    window = engine.getWindow();

    // Create and compile shaders
    Shader shaderProgram("simple","simple");
    shaderProgram.load();

    console->debugInfo=true;
    
    // Main loop
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {

        engine.updateDeltaTime();
        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.use();

        engine.drawCamera();
        engine.drawObjects();
        engine.drawConsole();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup

    // Terminate GLFW
    glfwTerminate();
    return 0;
}
