#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character
{
    GLuint     TextureID;
    glm::ivec2 Size; // Width, then height
    glm::ivec2 Bearing;
    FT_Pos     Advance;
};

class TextRenderer
{
  public:
    GLuint                    VAO;
    GLuint                    VBO;
    std::map<char, Character> characters;
    Shader *                  shader;
    // glm::mat4                 view;
    // glm::mat4                 projection;

    TextRenderer(const std::string &fontPath, Shader *fontShader, FT_UInt pixelSize);
    void renderText(const std::string &text, float x, float y, float scale,
                    glm::vec3 color);
    void renderTextMax(const std::string &text, float x, float y, float scale,
                       glm::vec3 color, int maxWidth);
};

#endif
