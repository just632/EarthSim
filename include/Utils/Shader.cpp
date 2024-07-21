#include "Shader.hpp"




GLint Shader::getUniformLocation(const std::string& name) {
    // Check if the uniform location is already cached
    auto it = uniformLocations.find(name);
    if (it != uniformLocations.end()) {
        return it->second;
    }

    // Get and cache the uniform location
    GLint location = glGetUniformLocation(programID, name.c_str());
    uniformLocations[name] = location;
    return location;
}