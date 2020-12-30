#include "game.h"
#include <glm/gtc/matrix_transform.hpp>

void Game::updateOrtho()
{
    this->projection = glm::ortho(-640.0f * this->zoomFactor, 640.0f * this->zoomFactor,
                                  -360.0f * this->zoomFactor, 360.0f * this->zoomFactor,
                                  0.1f, 1500.0f);
}
