cmake_minimum_required(VERSION 3.0)

project(apart-population LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BASE_SRCS
    main.cpp
    shader.h
    shader.cpp 
    table.h
    table.cpp
    stb_image.h
    stb_image.cpp
    "text_renderer.h"
    "text_renderer.cpp"
    game.h
    line_renderer.h
    line_renderer.cpp
    "check_error.h"
    "check_error.cpp")

add_executable(apart-population ${BASE_SRCS})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw-3.3.2)
add_subdirectory(libs/glad)
add_subdirectory(libs/glm)
add_subdirectory(libs/freetype-2.10.0)

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
     set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

target_link_libraries(apart-population glfw glad glm freetype)
