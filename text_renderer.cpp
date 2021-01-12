#include "text_renderer.h"
#include "game.h"

#include <string_view>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

TextRenderer::TextRenderer(const std::string &fontPath, Shader *fontShader, FT_UInt pixelSize)
    : shader(fontShader)
{
    // TODO: Put this constant at the level of the text renderer?
    constexpr int maxLineCharacters = 300; // Estimated number of characters per line
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4 * maxLineCharacters, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType library"
                  << std::endl;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, pixelSize);
    // FT_Set_Char_Size(face, 0, 12 * 64, 96, 96);

    // FIXME: Check if I should undo this after the font is loaded in
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
    
    int combinedWidth = 0;
    unsigned int maxHeight = 0;
    
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: Failed to load glyph" << std::endl;
            continue;
        }
        
        combinedWidth += face->glyph->bitmap.width;
        maxHeight = std::max(maxHeight, face->glyph->bitmap.rows);
    }
    
    atlasHeight = maxHeight;
    
    // Create texture atlas
    glGenTextures(1, &textureAtlas);
    glBindTexture(GL_TEXTURE_2D, textureAtlas);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, combinedWidth, maxHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    
    // Set texture options
    // TODO: Experiment with GL_NEAREST vs GL_LINEAR for a more (or less)
    // 8-bit esque look when small
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // TODO: Look into loading non-ASCII characters (like an ellipsis?)
    int x = 0;
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: Failed to load glyph" << std::endl;
            continue;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                        GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        // Now store character for later use
        Character character =
        {
            glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x,
            (float)x,
        };
        this->characters.insert(std::pair<char, Character>(c, character));
        
        x += face->glyph->bitmap.width;
    }
    
    atlasWidth = x;

    // Clear FreeType's resources now that we're done using them
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

float TextRenderer::renderTextPart(std::string_view text, float x, float y,
                                  float scale, glm::vec3 color)
{
    // Activate the corresponding render state
    shader->use();
    glUniform3f(glGetUniformLocation(shader->ID, "textColor"), color.x, color.y,
                color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    glm::mat4 model = glm::mat4(1.0f);
    shader->setMatrix("model", model);

    shader->setMatrix("projection", Game::main.projection);

    shader->setMatrix("view", Game::main.view);
    
    glBindTexture(GL_TEXTURE_2D, textureAtlas);
    
    struct Point
    {
        float x;
        float y;
        float s;
        float t;
    };
    
    Point allPoints[text.size()][6];
    
    // Iterate through all the characters
    for (int i = 0; i < text.size(); i++)
    {
        const char c = text[i];
        Character ch = characters[c];

        float xPos = x + ch.Bearing.x * scale;
        float yPos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        
        float leftX = ch.TextureX / atlasWidth;
        float rightX = (ch.TextureX + (float) ch.Size.x) / atlasWidth;
        // float rightX = 1.0f / (128.0f);
        float topY = ch.Size.y / (float) atlasHeight;
        
        // std::cout << "ch.TextureX: " << ch.TextureX
        //           << ", atlasWidth: " << atlasWidth << std::endl;
        //std::cout << "rightX: " << rightX
        //          << ", topY: " << topY << std::endl;
        
        allPoints[i][0] = {xPos,     yPos + h, leftX, 0.0f};
        allPoints[i][1] = {xPos,     yPos,     leftX, topY};
        allPoints[i][2] = {xPos + w, yPos,     rightX, topY};
        allPoints[i][3] = {xPos,     yPos + h, leftX, 0.0f};
        allPoints[i][4] = {xPos + w, yPos,     rightX, topY};
        allPoints[i][5] = {xPos + w, yPos + h, rightX, 0.0f};

        // Update content of VBO memory
        // glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        // Render quad
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        x += (ch.Advance >> 6) *
             scale; // Bitshift by 6 to get value in pixels (2*6 = 64)
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(allPoints), allPoints);
    glDrawArrays(GL_TRIANGLES, 0, 6 * text.size());
    
    return x;
}

void TextRenderer::renderText(std::string_view text, float x, float y,
                         float scale, glm::vec3 color)
{
    renderTextPart(text, x, y, scale, color);
}

// FIXME: Rewrite to use texture atlas
// maxWidth is in font pixels (texels)
void TextRenderer::renderTextMax(std::string_view text, float x, float y,
                                 float scale, glm::vec3 color, int maxWidth)
{
    const int period3Advance = 3 * (this->characters['.'].Advance / 64);
    
    // Calculate the maximum width of the string until maxWidth is reached
    int texelWidth = period3Advance;
    int i;
    for (i = 0; i < text.size(); i++)
    {
        char c = text[i];
        Character ch = this->characters[c];
        const int advance64 = ch.Advance / 64; 
        
        if (texelWidth + advance64 > maxWidth)
        {
            break;
        }
        
        texelWidth += advance64;
    }
    
    // std::cout << "i: " << i << ", text.size(): " << text.size() << ", maxWidth: " << maxWidth
    //           << ", texelWidth: " << texelWidth << std::endl;
    if (i >= text.size()) // Rendered text fits into maxWidth
    {
        renderTextPart(text, x, y, scale, color);
    }
    else // Rendered text is too long
    {
        int nextX = renderTextPart(text.substr(0, i), x, y, scale, color);
        renderTextPart("...", nextX, y, scale, color);
    }
}
