#ifndef TABLE_H
#define TABLE_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>

// This shall be a spreadsheet-like table of values
class Table
{
  public:
    // Mark upper-left corners
    int          xPos;
    int          yPos;
    unsigned int rows;
    unsigned int cols;
    GLuint       textVAO;

    Table(int xPos, int yPos, int rows, int cols, GLuint textVAO);

    void render();
};

#endif
