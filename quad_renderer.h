#ifndef QUAD_RENDERER_H
#define QUAD_RENDERER_H

#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "texture_2d.h"

struct VertexPoint
{
    float xCoord;
    float yCoord;
    
    float rColor;
    float gColor;
    float bColor;
    float aColor;
    
    // Texture coordinates
    float sCoord;
    float tCoord;
    
    float textureIndex;
};

struct AttributesQuad
{
    VertexPoint topRight;
    VertexPoint bottomRight;
    VertexPoint bottomLeft;
    VertexPoint topLeft;
};

// A batch renderer for quads with a color and sprite
class QuadRenderer
{
public:
    static constexpr int MAX_QUADS = 1000;
    
    Texture2D *texture1;
    Texture2D *texture2;
    
    GLuint VAO;
    GLuint VBO;
    
    QuadRenderer();
    // TODO: Implement?
    void prepareQuad(glm::vec2 position, float width, float height, glm::vec4 rgb, float textureIndex);
    void prepareQuad(AttributesQuad &input);
    void sendToGL();
    
private:
    std::array<AttributesQuad, MAX_QUADS> quadBuffer;
    // AttributesQuad *quadBuffer;
    int quadIndex;
    Shader shader;
};

#endif
