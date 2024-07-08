cmake_minimum_required(VERSION 3.28)
project(earth_sim VERSION 1.0.0)

find_package(OpenGL REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(earth_sim
    src/main.cpp
    src/glad.c
    include/common/controls.hpp
    include/common/controls.cpp
)

target_include_directories(earth_sim PRIVATE include)

target_link_libraries(earth_sim
glfw3
OpenGL::GL 
glut
)