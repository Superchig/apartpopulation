#ifndef LINE_RENDERER_H
#define LINE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

class LineRenderer
{
  public:
    GLuint  VAO;
    GLuint  VBO;
    Shader *shader;

    LineRenderer(Shader *shader);
    void drawLine(float xPosStart, float yPosStart, float xPosEnd, float yPosEnd);
};

#endif