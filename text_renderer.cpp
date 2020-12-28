#include "text_renderer.h"
#include "game.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

TextRenderer::TextRenderer(const std::string &fontPath, Shader *fontShader, FT_UInt pixelSize)
    : shader(fontShader)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType library"
                  << std::endl;
    }

    FT_Face face;
    if (FT_New_Face(ft, "fonts/Cantarell-Regular.otf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, pixelSize);

    // FIXME: Check if I should undo this after the font is loaded in
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    // TODO: Look into loading non-ASCII characters (like an ellipsis?)
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: Failed to load glyph" << std::endl;
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                     face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x};
        this->characters.insert(std::pair<char, Character>(c, character));
    }

    // Clear FreeType's resources now that we're done using them
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void TextRenderer::renderText(const std::string &text, float x, float y,
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

    // Iterate through all the characters
    for (char c : text)
    {
        Character ch = characters[c];

        float xPos = x + ch.Bearing.x * scale;
        float yPos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Update VBO for each character
        float vertices[6][4] = {
            {xPos, yPos + h, 0.0f, 0.0f},    {xPos, yPos, 0.0f, 1.0f},
            {xPos + w, yPos, 1.0f, 1.0f},

            {xPos, yPos + h, 0.0f, 0.0f},    {xPos + w, yPos, 1.0f, 1.0f},
            {xPos + w, yPos + h, 1.0f, 0.0f}};

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        x += (ch.Advance >> 6) *
             scale; // Bitshift by 6 to get value in pixels (2*6 = 64)
    }
    // glBindVertexArray(0);
    // glBindTexture(GL_TEXTURE_2D, 0);
}

// maxWidth is in font pixels
void TextRenderer::renderTextMax(const std::string &text, float x, float y,
                                 float scale, glm::vec3 color, int maxWidth)
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

    Character   period = characters['.'];
    const float periodAdvance = (period.Advance >> 6) * scale;
    const float ellipsisWidth = 4.0f * periodAdvance;
    const float maxRightPos = x + ((float)maxWidth * scale) - ellipsisWidth;

    // Iterate through all the characters
    for (char c : text)
    {
        Character ch = characters[c];

        float xPos = x + ch.Bearing.x * scale;
        float yPos = y - (ch.Size.y - ch.Bearing.y) * scale;

        if (x > maxRightPos)
        {
            // Render 3 periods/an ellipsis
            // ----------------------------
            for (int i = 0; i < 3; i++)
            {
                float xPos = x + period.Bearing.x * scale;
                float yPos = y - (period.Size.y - period.Bearing.y) * scale;

                float w = period.Size.x * scale;
                float h = period.Size.y * scale;

                // Update VBO for each character
                float vertices[6][4] = {{xPos, yPos + h, 0.0f, 0.0f},
                                        {xPos, yPos, 0.0f, 1.0f},
                                        {xPos + w, yPos, 1.0f, 1.0f},

                                        {xPos, yPos + h, 0.0f, 0.0f},
                                        {xPos + w, yPos, 1.0f, 1.0f},
                                        {xPos + w, yPos + h, 1.0f, 0.0f}};

                // Render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, period.TextureID);
                // Update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                // glBindBuffer(GL_ARRAY_BUFFER, 0);
                // Render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // Now advance cursors for next glyph (note that advance is
                // number of 1/64 pixels)
                x += (period.Advance >> 6) *
                     scale; // Bitshift by 6 to get value in pixels (2*6 = 64)
            }

            break;
        }

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Update VBO for each character
        float vertices[6][4] = {
            {xPos, yPos + h, 0.0f, 0.0f},    {xPos, yPos, 0.0f, 1.0f},
            {xPos + w, yPos, 1.0f, 1.0f},

            {xPos, yPos + h, 0.0f, 0.0f},    {xPos + w, yPos, 1.0f, 1.0f},
            {xPos + w, yPos + h, 1.0f, 0.0f}};

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of
        // 1/64 pixels)
        x += (ch.Advance >> 6) *
             scale; // Bitshift by 6 to get value in pixels (2*6 = 64)
    }
    // glBindVertexArray(0);
    // glBindTexture(GL_TEXTURE_2D, 0);
}
