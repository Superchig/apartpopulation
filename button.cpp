#include "button.h"
#include "game.h"
#include "sprite_renderer.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

Button::Button(float x, float y, int width, int height, Texture2D *texture,
               SpriteRenderer *spriteRenderer)
    : x(x), y(y), width(width), height(height), texture(texture),
      spriteRenderer(spriteRenderer)
{
}

void Button::draw()
{
    spriteRenderer->drawSprite(*texture, glm::vec2(x, y),
                               glm::vec2(width, height));
}

bool Button::hasInBounds(float x, float y) {
    const float halfWidth  = this->width / 2.0f;
    const float halfHeight = this->height / 2.0f;

    const float rightBound  = this->x + halfWidth;
    const float leftBound   = this->x - halfWidth;
    const float topBound    = this->y + halfHeight;
    const float bottomBound = this->y - halfHeight;

    return leftBound <= x && x <= rightBound && bottomBound <= y &&
           y <= topBound;
}