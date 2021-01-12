#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <ft2build.h>
#include <string_view>
#include FT_FREETYPE_H

struct Character
{
    glm::vec2 Size; // Width, then height
    glm::vec2 Bearing;
    FT_Pos    Advance;
    float     TextureX; // x offset of glyph in texture coordinates
};

class TextRenderer
{
private:
    // Returns the new x position after rendering
    float renderTextPart(std::string_view text, float x, float y, float scale,
                         glm::vec3 color);
    
public:
    GLuint                    VAO;
    GLuint                    VBO;
    GLuint                    textureAtlas;
    int                       atlasWidth;
    int                       atlasHeight;
    std::map<char, Character> characters;
    Shader *                  shader;
    // glm::mat4                 view;
    // glm::mat4                 projection;

    TextRenderer(const std::string &fontPath, Shader *fontShader, FT_UInt pixelSize);
    void renderText(std::string_view text, float x, float y, float scale,
                    glm::vec3 color);
    void renderTextMax(std::string_view text, float x, float y, float scale,
                       glm::vec3 color, int maxWidth);
    
    // TODO: Actually implement and then use
    void boxRenderText(const std::string &text, float x, float y, float width,
                       float height, float scale, glm::vec3 color);
};

#endif
