#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::mat4 getModleMatrix();
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


#endif