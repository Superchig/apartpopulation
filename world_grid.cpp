#include "world_grid.h"

#include "game.h"
#include "util.h"

void WorldGrid::draw()
{
    constexpr float PADDING       = 0.0f;
    constexpr float SQUARE_LENGTH = 30.0f;
    constexpr float HALF_LEN      = SQUARE_LENGTH * 0.5f;
    constexpr float MOVE          = SQUARE_LENGTH + PADDING;

    // TODO: Make some of these member variables
    const float centerX   = 100.0f;
    const float centerY   = 0.0f;
    const float startingX = centerX - ((cols - 1) * MOVE * 0.5f);
    const float startingY = centerY - ((rows - 1) * MOVE * 0.5f);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            for (int ind = 0; ind < nodes[i + rows * j]->occupants.size(); ind++)
            {
                if (nodes[i + rows * i]->occupants[ind]->isShown)
                {
                    const float squareX = startingX + (j * MOVE);
                    const float squareY = startingY - (i * -MOVE);
                    Game::main.quadRenderer->prepareQuad(
                        glm::vec2(squareX, squareY), SQUARE_LENGTH,
                        SQUARE_LENGTH, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
                        nodes[i + rows * j]->occupants[ind]->texture->ID);
                }
            }
        }
    }
}