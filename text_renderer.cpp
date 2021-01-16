#include "text_renderer.h"
#include "game.h"
#include "quad_renderer.h"

#include <string_view>
#include <iostream>
#include <climits>
#include <glm/gtc/matrix_transform.hpp>

TextRenderer::TextRenderer(const std::string &fontPath, Shader *fontShader, FT_UInt pixelSize)
    : shader(fontShader)
{
    // constexpr int maxLineCharacters = 300; // Estimated number of characters per line
    
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
    
    constexpr int ftLoadFlags = FT_LOAD_RENDER | FT_LOAD_COLOR;
    constexpr int imageFormat = GL_RGBA;
    
    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, ftLoadFlags))
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
    
    glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, combinedWidth, maxHeight, 0, imageFormat, GL_UNSIGNED_BYTE, nullptr);
    
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
        if (FT_Load_Char(face, c, ftLoadFlags))
        {
            std::cout << "ERROR::FREETYPE: Failed to load glyph" << std::endl;
            continue;
        }
        
        // Make an RGBA bitmap from FreeType's R bitmap
        const int newBitmapLength = face->glyph->bitmap.width * face->glyph->bitmap.rows * 4;
        unsigned char rgbaBitmap[newBitmapLength];
        for (int i = 0; i < newBitmapLength; i += 4)
        {
            rgbaBitmap[i]     = UCHAR_MAX; // Red
            rgbaBitmap[i + 1] = UCHAR_MAX; // Green
            rgbaBitmap[i + 2] = UCHAR_MAX; // Blue
            rgbaBitmap[i + 3] = face->glyph->bitmap.buffer[i / 4]; // Alpha
        }

        // glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows,
        //                 imageFormat, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                        imageFormat, GL_UNSIGNED_BYTE, rgbaBitmap);

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
    
    this->quadRenderIndex = (float)Game::main.quadRenderer->textureIDs.size();
    Game::main.quadRenderer->textureIDs.push_back(textureAtlas);
}

float TextRenderer::renderTextPart(std::string_view text, float x, float y,
                                  float scale, glm::vec3 color)
{
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
        
        // TODO: Rename leftX and rightX to be more descriptive/intuitive
        AttributesQuad glyphQuad;
        glyphQuad.topRight    = {xPos,     yPos + h,   color.r, color.g, color.b, 1.0f,   leftX,  0.0f,  quadRenderIndex};
        glyphQuad.bottomRight = {xPos,     yPos,       color.r, color.g, color.b, 1.0f,   leftX,  topY,  quadRenderIndex};
        glyphQuad.bottomLeft  = {xPos + w, yPos,       color.r, color.g, color.b, 1.0f,   rightX, topY,  quadRenderIndex};
        glyphQuad.topLeft     = {xPos + w, yPos + h,   color.r, color.g, color.b, 1.0f,   rightX, 0.0f,  quadRenderIndex};
        Game::main.quadRenderer->prepareQuad(0, glyphQuad);

        // Now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        x += (ch.Advance >> 6) *
             scale; // Bitshift by 6 to get value in pixels (2*6 = 64)
    }
    
    return x;
}

void TextRenderer::renderText(std::string_view text, float x, float y,
                         float scale, glm::vec3 color)
{
    renderTextPart(text, x, y, scale, color);
}

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
