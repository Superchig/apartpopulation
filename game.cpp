#include "game.h"
#include <glm/gtc/matrix_transform.hpp>

void Game::updateOrtho()
{
    const float halfWidth = windowWidth / 2.0f;
    const float halfHeight = windowHeight / 2.0f;
    this->projection = glm::ortho(-halfWidth * this->zoomFactor, halfWidth * this->zoomFactor,
                                  -halfHeight * this->zoomFactor, halfHeight * this->zoomFactor,
                                  0.1f, 1500.0f);
}
