#include "table.h"
#include "game.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Table::Table(float xPos, float yPos, int rows, TextRenderer *textRen)
    : xPos(xPos), yPos(yPos), rows(rows), textRen(textRen), data(rows)
{
    col_widths.fill(-1);
}

void Table::setItem(int row, int col, std::string value)
{
#ifndef NDEBUG
    if (row >= data.size())
    {
        std::cout << "ERROR::TABLE: Tried to change value at row " << row << " when number of rows is " << data.size() << std::endl;
        return;
    }
#endif
    
    data[row][col] = value;
}

void Table::setColWidth(int col, int width) { col_widths[col] = width; }

void Table::sendToRenderer()
{
    const float upperColHeight = textRen->atlasHeight;
    const float lowerColHeight = textRen->atlasHeight * 0.25f;
    const float colHeight = upperColHeight + lowerColHeight;
    
    // Calculate total column width
    float totalWidth = 0;
    for (int width : col_widths)
    {
        totalWidth += width;
    }
    
    // Top border for all columns
    Game::main.quadRenderer->prepareRightLine(xPos, yPos, totalWidth);
    
    float currentY = yPos - colHeight;
    for (int i = 0; i < data.size(); i++, currentY -= colHeight)
    {
        if (currentY > Game::main.topY)
        {
            continue;
        }
        else if (currentY + colHeight < Game::main.bottomY)
        {
            break;
        }
        
        glm::vec3 color =
            i == 0 ? glm::vec3(1.0f, 0.5f, 0.5f) : glm::vec3(1.0f, 1.0f, 1.0f);
        
        const auto &record = data[i];
        float textOrigin = currentY + lowerColHeight;
        float currentX = xPos;
        for (int j = 0; j < record.size(); currentX += col_widths[j], j++)
        {
            // std::cout << "col_widths[" << j << "]: " << col_widths[j] << std::endl;
            
            textRen->renderTextMax(record[j], currentX, textOrigin, 1.0f, color, col_widths[j]);
            
            // Left border for each cell
            Game::main.quadRenderer->prepareDownLine(currentX, currentY, -colHeight);
        }
        
        // Right border for last cell in a column
        Game::main.quadRenderer->prepareDownLine(currentX, currentY, -colHeight);
        
        // Bottom border for all columns
        Game::main.quadRenderer->prepareRightLine(xPos, currentY, totalWidth);
    }
}
