#include "table.h"

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
    data[row][col] = value;
}

void Table::setColWidth(int col, int width) { col_widths[col] = width; }

void Table::draw()
{
    const float SCALE_FACTOR = 0.00175f;
    // In height (in pixels) of font's bitmap
    const float DOWN_PADDING = 25.0f;
    const int   COL_WIDTH    = 400; // In font pixels

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
    const float downLengthBase    = (float)maxHeight * SCALE_FACTOR;
    const float downLengthPadding = DOWN_PADDING * SCALE_FACTOR;
    const float downLength        = downLengthBase + downLengthPadding;
    // const float scaledColWidth    = (float)COL_WIDTH * SCALE_FACTOR;
    for (int i = 0; i < data.size(); i++)
    {
        glm::vec3 color = i == 0 ? glm::vec3(1.0f, 0.5f, 0.5f) : glm::vec3(1.0f, 1.0f, 1.0f);

        const auto &record = data[i];

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

            currentX += scaledWidth;
        }

        currentY -= downLength;
    }
}
