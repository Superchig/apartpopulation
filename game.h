#ifndef GAME_H
#define GAME_H

#include <glm/glm.hpp>
#include <vector>
#include "calendar.h"

class HistoricalFigure;
class SpriteRenderer;
class Button;
class Table;

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
    
#pragma region Simulation
    std::vector<HistoricalFigure *> livingFigures;
    std::vector<HistoricalFigure *> deadFigures;
    
    Date date = {1, JAN};
#pragma endregion
    
    // TODO: Avoid using global state to make spreadTable accessible
    Table *spreadTable;
    
    void updateOrtho();
};

#endif
