cmake_minimum_required(VERSION 3.28)
project(earth_sim VERSION 1.0.0)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

add_executable(earth_sim
    src/main.cpp
    include/common/controls.hpp
    include/common/controls.cpp
    include/common/shader.hpp
    include/common/shader.cpp
    include/common/text2D.hpp
    include/common/text2D.cpp
    include/common/texture.hpp
    include/common/texture.cpp
)

target_include_directories(earth_sim PRIVATE include)

target_link_libraries(earth_sim
glfw3
OpenGL::GL
)