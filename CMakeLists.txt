cmake_minimum_required(VERSION 3.16)
project(OpenGLApp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/shaders.cpp
    src/window.cpp
    src/mesh.cpp
    src/lab-5/lab5.cpp
    # src/lab-4/lab4.cpp
    glad.c   
)
add_custom_target(run
    COMMAND ./OpenGLApp
    DEPENDS OpenGLApp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME}
    glfw
    OpenGL::GL
    glm::glm
)
