#include "table.h"
#include "game.h"

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Table::Table(float xPos, float yPos, int rows, TextRenderer *textRen,
             LineRenderer *lineRen)
    : xPos(xPos), yPos(yPos), rows(rows), textRen(textRen),
      lineRen(lineRen), data(rows)
{
    col_widths.fill(-1);
}

void Table::setItem(int row, int col, std::string value)
{
    data[row][col] = value;
}

void Table::setColWidth(int col, int width) { col_widths[col] = width; }

void Table::draw()
{
    const float SCALE_FACTOR = 1.0f;
    // const float SCALE_FACTOR = (1.0f / Game::main.window_height);
    // In height (in pixels) of font's bitmap
    const float DOWN_PADDING = 25.0f;
    // const int   COL_WIDTH    = 400; // In font pixels
    const int   COL_PADDING  = 50;  // In font pixels

    int maxHeight = -1;
    for (auto const &[ch, info] : textRen->characters)
    {
        int height = info.Size.y;
        if (height > maxHeight)
        {
            maxHeight = height;
        }
    }

    float currentY = yPos;

    // The minimum distance to move down to make a new line
    const float scaledDownLengthBase = (float)maxHeight * SCALE_FACTOR;
    const float scaledDownPadding    = DOWN_PADDING * SCALE_FACTOR;
    const float scaledDownLength     = scaledDownLengthBase + scaledDownPadding;

    // const float scaledColWidth    = (float)COL_WIDTH * SCALE_FACTOR;
    const float scaledColPadding = COL_PADDING * SCALE_FACTOR;

    // Calculate total column width
    float totalWidth = 0;
    for (int width : col_widths)
    {
        totalWidth += width + scaledColPadding;
    }
    const float scaledTotalWidth = totalWidth * SCALE_FACTOR;

    const float rightMostX = xPos + totalWidth;

    const float highestY =
        currentY + scaledDownLengthBase + (scaledDownPadding / 2.0f);
    lineRen->drawLine(xPos, highestY, rightMostX, highestY); // Top border

    const float lowestY = currentY - ((data.size() - 1) * scaledDownLength) -
                          (scaledDownPadding / 2.0f);
    // Left-most border
    lineRen->drawLine(xPos, highestY, xPos, lowestY);
    // Right-most border
    lineRen->drawLine(rightMostX, highestY, rightMostX, lowestY);

    // Draw left borders of columns
    float borderX = xPos;
    for (int j = 0; j < col_widths.size(); j++)
    {
        const float scaledWidth = col_widths[j] * SCALE_FACTOR + scaledColPadding;

        lineRen->drawLine(borderX, highestY, borderX, lowestY);

        borderX += scaledWidth;
    }

    for (int i = 0; i < data.size(); i++)
    {
        glm::vec3 color =
            i == 0 ? glm::vec3(1.0f, 0.5f, 0.5f) : glm::vec3(1.0f, 1.0f, 1.0f);

        const auto &record = data[i];

        const float bottomY = currentY - (scaledDownPadding / 2.0f);
        // Bottom borders for each row
        lineRen->drawLine(xPos, bottomY, xPos + scaledTotalWidth, bottomY);

        float currentX = xPos;

        for (int j = 0; j < record.size(); j++)
        {
            const std::string &item = record[j];

            const int   specificWidth = col_widths[j];
            const float scaledWidth   = specificWidth * SCALE_FACTOR;

            if (item.size() > 0)
            {
                textRen->renderTextMax(item, currentX, currentY, SCALE_FACTOR,
                                       color, specificWidth);
            }

            currentX += scaledWidth + scaledColPadding;
        }

        currentY -= scaledDownLength;
    }
}
