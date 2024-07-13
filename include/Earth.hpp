#ifndef EARTH_HPP
#define EARTH_HPP

#include "tiny_gltf.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Earth
{
public:
    // Get the singleton instance of Earth
    static Earth &getInstance()
    {
        static Earth instance;
        return instance;
    };

    // Load the GLB model from a file
    bool loadModel(const std::string &filename);

    // Render the Earth model
    void render();

    void load(){
        loadModel("/home/m/Code/cpp/EarthSim/modles/Earth.glb");
        std::cout<<"Earth Loaded"<<std::endl;
    }
    

private:
    // Private constructor and destructor
    Earth() : modelLoaded(false)
    {
        
    }
    ~Earth()
    {
        // Delete VAOs
        for (auto &vao : primitiveVAOs)
        {
            glDeleteVertexArrays(1, &vao.second);
        }
    }

    // Load the GLB model using tinygltf
    bool loadGLBModel(tinygltf::Model &model, const std::string &filename);

    // Initialize OpenGL buffers for the model
    void initializeGLBuffers(const tinygltf::Model &model);

    // Render the model using OpenGL
    void renderModel(const tinygltf::Model &model);

    tinygltf::Model model; // Model data
    bool modelLoaded;      // Flag to check if model is loaded

    // Store VAOs for each primitive
    std::unordered_map<int, GLuint> primitiveVAOs;
    std::unordered_map<int, GLsizei> primitiveIndexCounts;
};

#endif // EARTH_HPP