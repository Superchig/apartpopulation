#ifndef TABLE_H
#define TABLE_H

#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "shader.h"
#include "text_renderer.h"
#include "button.h"

// This shall be a spreadsheet-like table of values
class Table
{
  public:
    static constexpr size_t COLS = 5;

    // Mark upper-left corners
    float                                      xPos;
    float                                      yPos;
    bool                                       isActive;
    // The distance from the top of the first rendered row to the "invisible position" of the first row
    float                                      yOffset;
    float                                      yDownLength;
    size_t                                     rows;
    TextRenderer *                             textRen;
    std::vector<std::array<std::string, COLS>> data;
    // TODO: Rename in its own commit
    std::array<int, COLS>                      col_widths;

    Button *scrollButton;

    Table(float xPos, float yPos, int rows, TextRenderer *textRen);

    void setItem(int row, int col, std::string value);
    void setColWidth(int col, int width);
    void sendToRenderer();
};

#endif
