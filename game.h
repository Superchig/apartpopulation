#ifndef GAME_H
#define GAME_H

#include <glm/glm.hpp>
#include <vector>

class SpriteRenderer;
class Button;

// Global state
class Game
{
  public:
    static Game main;

    float       eyeX = 0.0f;
    float       eyeY = 0.0f;
    float       eyeZ = 1000.0f;
    float       playerX      = 0.0f;
    float       playerY      = 0.0f;
    const float playerChange = 5.0f;
    float zoomFactor = 1.0f;

    float mouseX;
    float mouseY;

    std::vector<Button *> buttons;

    int window_width  = 1280;
    int window_height = 720;

    glm::mat4 view;
    glm::mat4 projection;

    void updateOrtho();
};

#endif
