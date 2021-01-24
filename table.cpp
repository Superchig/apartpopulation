#include "table.h"
#include "game.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Table::Table(float xPos, float yPos, int rows, TextRenderer *textRen)
    : xPos(xPos), yPos(yPos), isActive(true), yOffset(0.0f), yDownLength(0.0f), rows(rows),
      textRen(textRen), data(rows), scrollButton(nullptr)
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
    
    const float dataWorldHeight = colHeight * data.size();
    
    const float heightProportion = yDownLength / dataWorldHeight;
    scrollButton->height = heightProportion * yDownLength;
    // const float scrollProportion = (yPos - scrollButton->y - scrollButton->height / 2.0f) / yDownLength;
    
    const float bottomY = yPos - yDownLength;
 
    // Calculate total column width
    float totalWidth = 0;
    for (int width : col_widths)
    {
        totalWidth += width;
    }

    float currentY = yPos + yOffset - colHeight;
    for (int i = 0; i < data.size(); i++, currentY -= colHeight)
    {
        if (currentY > Game::main.topY || currentY > yPos)
        {
            continue;
        }
        else if (currentY + colHeight < bottomY || currentY + colHeight < Game::main.bottomY)
        {
            break;
        }

        // Top border for all columns
        Game::main.quadRenderer->prepareRightLine(xPos, currentY + colHeight,
                                                  totalWidth);

        glm::vec3 color =
            i == 0 ? glm::vec3(1.0f, 0.5f, 0.5f) : glm::vec3(1.0f, 1.0f, 1.0f);
        
        const auto &record = data[i];
        float textOrigin = currentY + lowerColHeight;
        float currentX = xPos;
        for (int j = 0; j < record.size(); currentX += col_widths[j], j++)
        {
            textRen->renderTextMax(record[j], currentX, textOrigin, 1.0f, color, col_widths[j]);
            
            // Left border for each cell
            Game::main.quadRenderer->prepareDownLine(currentX, currentY, -colHeight);
        }
        
        // Right border for last cell in a column
        Game::main.quadRenderer->prepareDownLine(currentX, currentY, -colHeight);
        
        // Bottom border for all columns
        Game::main.quadRenderer->prepareRightLine(xPos, currentY, totalWidth);
    }
    
    // Update the position of the scroll button
    // ----------------------------------------
    scrollButton->x = xPos + totalWidth + scrollButton->width / 2.0f;
    
    const float downProp = yOffset / dataWorldHeight;
    const float scrollYTop = yPos - (downProp * yDownLength);
    const float scrollY = scrollYTop - (scrollButton->height / 2.0f);
    
    scrollButton->y = scrollY;
    
    if (scrollButton->isClicked)
    {
        const float halfHeight = scrollButton->height / 2.0f;
    
        scrollButton->y += Game::main.deltaMouseY;
        if (scrollButton->y > yPos - halfHeight)
        {
            scrollButton->y = yPos - halfHeight;
        }
        else if (scrollButton->y < bottomY + halfHeight)
        {
            scrollButton->y = bottomY + halfHeight;
        }
        
        yOffset = dataWorldHeight * (yPos - scrollButton->y - (scrollButton->height / 2.0f)) / yDownLength;
    }
}
