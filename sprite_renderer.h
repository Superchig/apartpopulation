#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "texture_2d.h"

class SpriteRenderer
{
public:
    Shader shader;
    GLuint quadVAO;
    
    SpriteRenderer(Shader &shader);
    ~SpriteRenderer();
    
    void drawSprite(Texture2D &texture, glm::vec2 position,
                    glm::vec2 size = glm::vec2(10.0f, 10.0f),
                    float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    
private:
    void initRenderData();
};

#endif
