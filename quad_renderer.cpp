#include "quad_renderer.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cstring>
#include "check_error.h"
#include "game.h"

QuadRenderer::QuadRenderer() : quadIndex(0), shader("shaders/quad.vert", "shaders/quad.frag")
{
    GLuint quadIBO;
    glCheckError();
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glGenBuffers(1, &quadIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
    
    glCheckError();
    
    glBufferData(GL_ARRAY_BUFFER, MAX_QUADS * sizeof(AttributesQuad), nullptr, GL_DYNAMIC_DRAW);
    
    glCheckError();
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPoint), (void *)offsetof(VertexPoint, xCoord));
    glEnableVertexAttribArray(0);
    // rgba values for color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPoint), (void *)offsetof(VertexPoint, rColor));
    glEnableVertexAttribArray(1);
    // s and t coordinates for texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPoint), (void *)offsetof(VertexPoint, sCoord));
    glEnableVertexAttribArray(2);
    // Texture index
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VertexPoint), (void *)offsetof(VertexPoint, textureIndex));
    glEnableVertexAttribArray(3);
    
    glCheckError();
    
    // unsigned int quadVertices[] = {
    //     0, 1, 3,
    //     1, 2, 3,
    //     4, 5, 7,
    //     5, 6, 7
    // };
    unsigned int quadIndices[MAX_QUADS * 6];
    for (int i = 0; i < MAX_QUADS; i++)
    {
        const int rightOffset = 4 * i;
        const int leftOffset = 6 * i;
        
        quadIndices[leftOffset + 0] = rightOffset + 0;
        quadIndices[leftOffset + 1] = rightOffset + 1;
        quadIndices[leftOffset + 2] = rightOffset + 3;
        
        quadIndices[leftOffset + 3] = rightOffset + 1;
        quadIndices[leftOffset + 4] = rightOffset + 2;
        quadIndices[leftOffset + 5] = rightOffset + 3;
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// void QuadRenderer::prepareQuad(glm::vec2 position, float width, float height,
//                                glm::vec4 rgb, float textureIndex)
// {
//     AttributesQuad *quad = &this->quadBuffer[quadIndex];
// }

void QuadRenderer::prepareQuad(AttributesQuad &input)
{
    quadBuffer[quadIndex] = input;
    // memcpy(&quadBuffer[quadIndex], &input, sizeof(input));
    quadIndex++;
}

void QuadRenderer::sendToGL()
{
    shader.use();
    shader.setMatrix("MVP", Game::main.projection * Game::main.view);
    
    GLint location = glGetUniformLocation(shader.ID, "batchQuadTextures");
    int samplers[2] = {0, 1};
    glUniform1iv(location, 2, samplers);
    
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture1->ID);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, texture2->ID);
    // glBindSampler(0, texture1->ID);
    // glBindSampler(1, texture2->ID);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Must bind VBO before glBufferSubData
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, quadIndex * sizeof(AttributesQuad), &quadBuffer[0]);
    glDrawElements(GL_TRIANGLES, quadIndex * 6, GL_UNSIGNED_INT, nullptr);
}
