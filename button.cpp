#include "button.h"
#include "game.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

Button::Button(float x, float y, int width, int height, Texture2D *texture,
               TextRenderer *textRen, std::string text, float vertPadding)
    : x(x), y(y), width(width), height(height), texture(texture),
      textRen(textRen), text(text), vertPadding(vertPadding)
{
}

void Button::sendToRenderer()
{
    // spriteRenderer->drawSprite(*texture, glm::vec2(x, y),
    //                            glm::vec2(width, height));
    
    // Works except for the fact that text renders too early
    Game::main.quadRenderer->prepareQuad(glm::vec2(x, y), width, height, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture->ID);
    
    // TODO: Dynamically calculate the left increase, rather than manually assigning vertical padding
    const float left = x - (width / 2.0f) + vertPadding;
    if (!this->text.empty())
    {
        textRen->renderText(text, left, y - textRen->atlasHeight / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

bool Button::hasInBounds(float x, float y)
{
    const float halfWidth  = this->width / 2.0f;
    const float halfHeight = this->height / 2.0f;

    const float rightBound  = this->x + halfWidth;
    const float leftBound   = this->x - halfWidth;
    const float topBound    = this->y + halfHeight;
    const float bottomBound = this->y - halfHeight;

    return leftBound <= x && x <= rightBound && bottomBound <= y &&
           y <= topBound;
}
