#ifndef GAME_H
#define GAME_H

#include <glm/glm.hpp>

// Global state
struct Game
{
    static Game main;

    float       eyeX = 0.0f;
    float       eyeY = 0.0f;
    float       eyeZ = 3.0f;
    float       playerX      = 0.0f;
    float       playerY      = 0.0f;
    const float playerChange = 0.01f;

    int window_width  = 1280;
    int window_height = 720;

    glm::mat4 view;
    glm::mat4 projection;
};

#endif
