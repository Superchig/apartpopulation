#ifndef GAME_H
#define GAME_H

#include <glm/glm.hpp>
#include <vector>
#include "calendar.h"
#include "quad_renderer.h"

class HistoricalFigure;
class Button;
class Table;
class Grid;
class WorldGrid;

// Global state
class Game
{
  public:
    static Game main;

    float       eyeX = 0.0f;
    float       eyeY = 0.0f;
    float       eyeZ = 1000.0f;
    // float       playerX      = 0.0f;
    // float       playerY      = 0.0f;
    // const float playerChange = 5.0f;
    float       zoomFactor   = 1.0f;

    float mouseX;
    float mouseY;
    float deltaMouseX = 0.0f;
    float deltaMouseY = 0.0f;

    std::vector<Button *> buttons;

    int windowWidth  = 1280;
    int windowHeight = 720;
    
    float topY;
    float bottomY;
    float leftX;
    float rightX;
    
    glm::mat4 view;
    glm::mat4 projection;
    
    QuadRenderer *quadRenderer;

#pragma region Simulation
    std::vector<HistoricalFigure *> livingFigures;
    std::vector<HistoricalFigure *> deadFigures;

    int marriageEligible;
    
    Date date = {1, JAN};
    
    Grid *landGrid;
    WorldGrid *worldGrid;
#pragma endregion
    
    // TODO: Avoid using global state to make spreadTable accessible
    Table *spreadTable;
    
    void updateOrtho();
};

#endif
