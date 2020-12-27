cmake_minimum_required(VERSION 3.0)

project(apart-population LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(apart-population main.cpp Shader.h Shader.cpp stb_image.h stb_image.cpp)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw-3.3.2)
add_subdirectory(libs/glad)
add_subdirectory(libs/glm)
add_subdirectory(libs/freetype-2.10.0)

target_link_libraries(apart-population glfw glad glm freetype)
