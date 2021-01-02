#include "sprite_renderer.h"
#include "game.h"
#include <glm/gtc/matrix_transform.hpp>

SpriteRenderer::SpriteRenderer(Shader &shader) : shader(shader)
{
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::initRenderData()
{
    // Configure VAO/VBO
    GLuint VBO;
    float vertices[] = {
        // Position // Texture
        -0.5f,  0.5f, 0.0f, 1.0f, // Top left
         0.5f, -0.5f, 1.0f, 0.0f, // Bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, // Bottom left
        
        -0.5f,  0.5f, 0.0f, 1.0f, // Top left
         0.5f,  0.5f, 1.0f, 1.0f, // Top right
         0.5f, -0.5f, 1.0f, 0.0f  // Bottom right
    };
    
    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(this->quadVAO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SpriteRenderer::drawSprite(Texture2D &texture, glm::vec2 position,
                                glm::vec2 size, float rotate, glm::vec3 color)
{
    this->shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    
    model = glm::scale(model, glm::vec3(size, 1.0f));
    
    glm::mat4 MVP = Game::main.projection * Game::main.view * model;
    this->shader.setMatrix("MVP", MVP);
    // TODO: Check if this method is implemented correctly
    this->shader.setVector3f("spriteColor", color);
    
    glActiveTexture(GL_TEXTURE0);
    texture.bind();
    
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
