#ifndef QUAD_RENDERER_H
#define QUAD_RENDERER_H

#include <array>
#include <vector>
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

// Store the quads before a draw call
class Batch
{
public:
    static constexpr int MAX_QUADS = 10000;
    
    // TODO: Look into decoupling # of quads that can be rendered with # of textures that can be rendered in one batch
    std::array<AttributesQuad, MAX_QUADS> quadBuffer;
    int quadIndex = 0;
};

// A batch renderer for quads with a color and sprite
class QuadRenderer
{
public:
    // Should be GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS for release, but
    // would need to figure out how to use that value in the fragment shader.
    // NOTE: Fragment shader also has hard-coded value that must match this.
    static constexpr int MAX_TEXTURES_PER_BATCH = 48;
    
    std::vector<GLuint> textureIDs;

    GLuint VAO;
    GLuint VBO;
    
    QuadRenderer();
    // TODO: Implement?
    void prepareQuad(glm::vec2 position, float width, float height, glm::vec4 rgb, int textureID); // Specify texture ID rather than index?
    // NOTE: Directly sending a texture index rather than ID can result in the wrong texture being drawn (due to being in the wrong batch)
    void prepareQuad(int batchIndex, AttributesQuad &input);
    void sendToGL();

private:
    std::vector<Batch> batches;
    Shader shader;
    
    void flush(const Batch &batch);
};

#endif
