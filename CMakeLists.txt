cmake_minimum_required(VERSION 3.28)
project(earth_sim VERSION 1.0.0)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(glm CONFIG REQUIRED)
add_subdirectory("include/tinygltf")
include_directories(${GLEW_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} "include" "include/tinygltf")
link_libraries(${GLEW_LIBRARIES} 
               ${FREETYPE_LIBRARIES} 
               glfw3
               OpenGL::GL
               tinygltf
               )


add_executable(earth_sim
    src/main.cpp
    include/Engine.cpp
    include/Engine.cpp
    include/Earth.cpp
    include/Earth.cpp
    include/Utils/Shader.cpp
    include/Utils/Shader.hpp
    include/Utils/wgs84.cpp
    include/Utils/wgs84.hpp
    include/Utils/Camera.hpp
    include/Utils/FirstPersonCamera.hpp
    include/Utils/FirstPersonCamera.cpp
    include/FlyingObjects/FlyingObject.hpp
    include/FlyingObjects/Rock.hpp
    include/FlyingObjects/Rock.cpp
    include/Utils/Console.cpp
    include/Utils/Console.hpp
    # include/common/controls.hpp
    # include/common/controls.cpp
    # include/common/shader.hpp
    # include/common/shader.cpp
    # include/common/text2D.hpp
    # include/common/text2D.cpp
    # include/common/texture.hpp
    # include/common/texture.cpp
    include/tinygltf/json.hpp
    include/tinygltf/stb_image_write.h
    include/tinygltf/stb_image.h
    include/tinygltf/tiny_gltf.h
)
