#include "line_renderer.h"
#include "game.h"
#include "check_error.h"

LineRenderer::LineRenderer(Shader *shader) : shader(shader)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 6, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);
}

void LineRenderer::drawLine(float xPosStart, float yPosStart, float xPosEnd,
                            float yPosEnd)
{
    float vertices[] = {
        xPosStart, yPosStart, 1.0f, 1.0f, 1.0f, 1.0f,
        xPosEnd, yPosEnd, 1.0f, 1.0f, 1.0f, 1.0f
    };

    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 MVP   = Game::main.projection * Game::main.view * model;
    shader->setMatrix("MVP", MVP);

    glCheckError();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glCheckError();
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_LINES, 0, 2);

    glCheckError();

    // float vertices[] = {
    //     xPosStart, yPosStart, 0.5f, 0.5f, 0.5f,
    //     xPosEnd, yPosEnd, 0.5f, 0.5f, 0.5f
    // };

    // shader->use();
    // glm::mat4 model = glm::mat4(1.0f);
    // shader->setMatrix("model", model);
    // shader->setMatrix("view", Game::main.view);
    // shader->setMatrix("projection", Game::main.projection);

    // glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    // glDrawArrays(GL_LINES, 0, 2);
}
