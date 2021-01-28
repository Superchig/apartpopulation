#include "land_plot.h"

#include "game.h"
#include "util.h"

FamilyNode::FamilyNode(FamilyNode *leader, HistoricalFigure *head,
                       LandPlot *plot)
    : head(head), leader(leader), plot(plot), defunct(false)
{
    if (this->head != nullptr)
    {
        this->head->family = this;
    }
}

Grid::Grid(TextRenderer *textRen, int rows, int cols)
    : rows(rows), cols(cols), land(rows * cols), textRen(textRen)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            LandPlot currentPlot = this->plot(j, i);
            currentPlot.mapX = j;
            currentPlot.mapY = i;
        }
    }
}

void Grid::draw()
{
    constexpr float PADDING = 10.0f;
    constexpr float SQUARE_LENGTH = 30.0f;
    constexpr float HALF_LEN = SQUARE_LENGTH * 0.5f;
    constexpr float MOVE = SQUARE_LENGTH + PADDING;
    
    const float centerX = 0.0f;
    const float centerY = 0.0f;
    const float startingX = centerX - ((cols - 1) * MOVE * 0.5f);
    const float startingY = centerY - ((rows - 1) * MOVE * 0.5f);
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            const float squareX = startingX + (j * MOVE);
            const float squareY = startingY - (i * -MOVE);
            Game::main.quadRenderer->prepareQuad(glm::vec2(squareX, squareY), SQUARE_LENGTH, SQUARE_LENGTH,
                                                 glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), Game::main.quadRenderer->whiteTextureID);
            textRen->renderText(std::to_string(plot(j, i).rootFamilies.size()),
                                squareX - HALF_LEN, squareY - HALF_LEN, 1.0f, glm::vec3(0.5f, 0.5f, 0.5f));
        }
    }
}

LandPlot &Grid::plot(int x, int y)
{
    return land[x + rows * y];
}
