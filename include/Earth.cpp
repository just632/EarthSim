#include "Earth.hpp"
#include "tiny_gltf.h"


// Load the GLB model from a file
bool Earth::loadModel(const std::string& filename) {
    modelLoaded = loadGLBModel(model, filename);
    if (modelLoaded) {
        initializeGLBuffers(model);
    }
    return modelLoaded;
}

// Load the GLB model using tinygltf
bool Earth::loadGLBModel(tinygltf::Model& model, const std::string& filename) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }

    return res;
}

// Initialize OpenGL buffers for the model
void Earth::initializeGLBuffers(const tinygltf::Model& model) {
    // Iterate through each mesh in the model
    for (size_t i = 0; i < model.meshes.size(); ++i) {
        const auto& mesh = model.meshes[i];
        for (size_t j = 0; j < mesh.primitives.size(); ++j) {
            const auto& primitive = mesh.primitives[j];

            GLuint vao;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Load vertex positions
            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const auto& accessor = model.accessors[primitive.attributes.at("POSITION")];
                const auto& bufferView = model.bufferViews[accessor.bufferView];
                const auto& buffer = model.buffers[bufferView.buffer];

                GLuint vbo;
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, bufferView.byteLength, &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, accessor.ByteStride(bufferView), (void*)0);
                glEnableVertexAttribArray(0);
            }

            // Load indices
            if (primitive.indices >= 0) {
                const auto& accessor = model.accessors[primitive.indices];
                const auto& bufferView = model.bufferViews[accessor.bufferView];
                const auto& buffer = model.buffers[bufferView.buffer];

                GLuint ebo;
                glGenBuffers(1, &ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferView.byteLength, &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);

                // Store index count
                primitiveIndexCounts[i * mesh.primitives.size() + j] = accessor.count;
            }

            // Store the VAO
            primitiveVAOs[i * mesh.primitives.size() + j] = vao;

            glBindVertexArray(0);
        }
    }
}

// Render the Earth model
void Earth::render() {
    if (modelLoaded) {
        renderModel(model);
    }
}

// Render the model using OpenGL
void Earth::renderModel(const tinygltf::Model& model) {
    for (size_t i = 0; i < model.meshes.size(); ++i) {
        const auto& mesh = model.meshes[i];
        for (size_t j = 0; j < mesh.primitives.size(); ++j) {
            glBindVertexArray(primitiveVAOs[i * mesh.primitives.size() + j]);
            const auto& primitive = mesh.primitives[j];
            glDrawElements(GL_TRIANGLES, primitiveIndexCounts[i * mesh.primitives.size() + j], GL_UNSIGNED_INT, 0);
        }
    }
}