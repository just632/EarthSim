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
    include/Utils/Shader.cpp
    include/Utils/Shader.hpp

    include/Utils/Timer.hpp
    include/Utils/Timer.cpp
    include/Utils/Console.cpp
    include/Utils/Console.hpp
    include/Utils/Window.cpp
    include/Utils/Window.hpp

    include/Utils/Physics.hpp
    include/Utils/Window.hpp
    include/static/wgs84.cpp
    include/static/wgs84.hpp
    include/Cameras/Camera.hpp  
    include/Cameras/FirstPersonCamera.hpp
    include/Cameras/FirstPersonCamera.cpp
     include/Objects/Object.hpp
     include/Objects/Object.cpp
     include/Objects/Ellipsoid.hpp
    include/common/config.h
    include/tinygltf/json.hpp
    include/tinygltf/stb_image_write.h
    include/tinygltf/stb_image.h
    include/tinygltf/tiny_gltf.h
)

add_executable(wgs_test
    src/wgs_test.cpp
    include/Utils/Physics.hpp
    include/static/wgs84.cpp
    include/static/wgs84.hpp
)

