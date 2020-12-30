#include <iostream>
#include <filesystem>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader.h"
#include "text_renderer.h"
#include "table.h"
#include "game.h"
#include "line_renderer.h"
#include "check_error.h"

// const unsigned int WINDOW_WIDTH  = 800;
// const unsigned int WINDOW_HEIGHT = 600;

GLuint                    VAO_FONT;
GLuint                    VBO_FONT;
GLuint                    VAO_TRIG;
GLuint                    VBO_TRIG;
GLuint                    VAO_RECT;
GLuint                    VBO_RECT;
// float                     eyeX         = 0.0f;
// float                     eyeY         = 0.0f;
// float                     eyeZ         = 3.0f;
// float                     playerX      = 0.0f;
// float                     playerY      = 0.0f;
// const float               playerChange = 0.01f;

Game Game::main;

float eyeChange(float eyeZ) { return eyeZ / 100.0f; }

void drawTriangle(Shader &shader, float centerX, float centerY, float scale,
                  float rotateDeg)
{
    shader.use();

    glm::vec3 eye    = glm::vec3(Game::main.eyeX, Game::main.eyeY, Game::main.eyeZ);
    glm::vec3 center = eye + glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view   = glm::lookAt(eye, center, up);
    shader.setMatrix("view", view);

    // Draw triangle
    // -------------
    glBindVertexArray(VAO_TRIG);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(centerX, centerY, 0.0f));
    model = glm::rotate(model, glm::radians(rotateDeg),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

    shader.setMatrix("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawRectangle(Shader &shader, float centerX, float centerY, float width,
                   float height, float rotateDeg)
{
    shader.use();

    glm::vec3 eye    = glm::vec3(Game::main.eyeX, Game::main.eyeY, Game::main.eyeZ);
    glm::vec3 center = eye + glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view   = glm::lookAt(eye, center, up);
    shader.setMatrix("view", view);

    // Draw rectangle
    // --------------
    glBindVertexArray(VAO_RECT);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(centerX, centerY, 0.0f));
    model = glm::rotate(model, glm::radians(rotateDeg),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    shader.setMatrix("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main()
{
    std::cout << "Hello, world!" << std::endl;
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // FIXME: Setup debug output with ARB_debug_output extension
    // as per https://learnopengl.com/In-Practice/Debugging
    // TODO: Move to GLEW (https://github.com/nigels-com/glew) to
    // dynamically access extensions

    // int flags;
    // glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    // if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    // {
    //     std::cout << "Initializing debug!" << std::endl;
    // }
#endif

    glfwWindowHintString(GLFW_X11_CLASS_NAME, "OpenGL");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "OpenGL");

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window =
        glfwCreateWindow(Game::main.window_width, Game::main.window_height, "Platformer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << '\n';
        return -1;
    }

    Shader shaderProgram("shader.vert", "shader.frag");

    const float SIDE_LENGTH = 1.0f;
    const float HALF_SIDE   = SIDE_LENGTH / 2.0f;
    const float TRI_HEIGHT  = SIDE_LENGTH * sqrtf(3.0f) / 2.0f;
    const float HALF_HEIGHT = TRI_HEIGHT / 2.0f;

    // Equilateral triangle
    float triVertices[] = {
        0.0f,       HALF_HEIGHT,  0.0f, 1.0f, 0.0f, 0.0f, // Top
        -HALF_SIDE, -HALF_HEIGHT, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom left
        HALF_SIDE,  -HALF_HEIGHT, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom right
    };

    float rectVertices[] = {
        // Positions        // Colors
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // Top right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, // Top left
    };

    unsigned int rectIndices[] = {
        0, 1, 3, // First triangle
        1, 2, 3, // Second triangle
    };

    glGenVertexArrays(1, &VAO_TRIG);
    glBindVertexArray(VAO_TRIG);

    glGenBuffers(1, &VBO_TRIG);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TRIG);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Initialize vertices for rectangle
    // ---------------------------------
    glGenVertexArrays(1, &VAO_RECT);
    glBindVertexArray(VAO_RECT);

    glGenBuffers(1, &VBO_RECT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_RECT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint EBO_RECT;
    glGenBuffers(1, &EBO_RECT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_RECT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIndices), rectIndices,
                 GL_STATIC_DRAW);

    // Initialize font vertex array
    // ---------------------------
    glGenVertexArrays(1, &VAO_FONT);
    glBindVertexArray(VAO_FONT);

    glGenBuffers(1, &VBO_FONT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_FONT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Set projection matrix outside of render loop
    // -----------------------------------
    shaderProgram.use();
    Game::main.projection = glm::perspective(
        glm::radians(45.0f), (float)Game::main.window_width / (float)Game::main.window_height, 0.1f,
        100.0f);
    GLint projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(Game::main.projection));

    // Declare camera manipulation variables
    // -------------------------------------
    const float zChange = 0.1f;

    // Initialize and set up table and text
    // ------------------------------------
    Shader       textShader("text.vert", "text.frag");
    TextRenderer textRen("fonts/Cantarell-Regular.otf", &textShader, 48);
    Shader       shader2d("2d_shader.vert", "2d_shader.frag");
    LineRenderer lineRen(&shader2d);

    Table spreadTable(-2.0f, 0.75f, 10, &textRen);
    spreadTable.setColWidth(0, 400);
    spreadTable.setColWidth(1, 800);
    spreadTable.setItem(0, 0, "Name");
    spreadTable.setItem(0, 1, "Age");

    spreadTable.setItem(1, 0, "Noble 1");
    spreadTable.setItem(1, 1, "30");

    spreadTable.setItem(2, 0, "Noble 2");
    spreadTable.setItem(2, 1, "52");

    spreadTable.setItem(3, 0, "Noble 3");
    spreadTable.setItem(3, 1, "22");

    spreadTable.setItem(4, 0, "A line was skipped!");
    spreadTable.setItem(4, 1, "Ten");

    glCheckError();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
        {
            Game::main.eyeZ -= zChange;
            std::cout << "Game::main.eyeZ: " << Game::main.eyeZ << std::endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            Game::main.Game::main.eyeZ += zChange;
            std::cout << "Game::main.eyeZ: " << Game::main.eyeZ << std::endl;
        }

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        {
            Game::main.eyeY += eyeChange(Game::main.eyeZ);
        }
        else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        {
            Game::main.eyeY -= eyeChange(Game::main.eyeZ);
        }

        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        {
            Game::main.eyeX -= eyeChange(Game::main.eyeZ);
        }
        else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            Game::main.eyeX += eyeChange(Game::main.eyeZ);
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            Game::main.playerY += Game::main.playerChange;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            Game::main.playerY -= Game::main.playerChange;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            Game::main.playerX += Game::main.playerChange;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            Game::main.playerX -= Game::main.playerChange;
        }

        glm::vec3 eye    = glm::vec3(Game::main.eyeX, Game::main.eyeY, Game::main.eyeZ);
        glm::vec3 center = eye + glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);
        Game::main.view  = glm::lookAt(eye, center, up);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render text
        // -----------
        // textRen.renderText("Hello, world!", 0.0f, 0.0f, 0.005f,
        //                    glm::vec3(0.5f, 0.8f, 0.2f));
        // textRen.renderText("Line two!", 0.0f, -0.3f, 0.005f,
        //                    glm::vec3(0.5f, 0.8f, 0.2f));

        double xPos;
        double yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        std::stringstream sstream;
        sstream << "Mouse cursor: " << xPos << ", " << yPos;
        textRen.renderText(sstream.str(), -1.0f, -1.0f, 0.005f,
                           glm::vec3(1.0f, 1.0f, 1.0f));

        // const double xClip = (xPos / (Game::main.window_width / 2.0f)) - 1.0f;
        // const double yClip = (yPos / (Game::main.window_height / 2.0f)) - 1.0f;
        // sstream.str(std::string());
        // sstream << "Mouse NDC: " << xClip << ", " << yClip;
        // textRen.renderText(sstream.str(), -1.0f, -0.5f, 0.005f,
        //                    glm::vec3(1.0f, 1.0f, 1.0f));

        spreadTable.draw();

        lineRen.drawLine(0.0f, 0.0f, 0.0f, 1.0f);

        // Test out rendering a line with the vertices (and shader)
        // for a triangle
        // shaderProgram.use();
        // shaderProgram.setMatrix("model", glm::mat4(1.0f));
        // shaderProgram.setMatrix("view", Game::main.view);
        // shaderProgram.setMatrix("projection", Game::main.projection);
        // glBindVertexArray(VAO_TRIG);
        // glDrawArrays(GL_LINES, 0, 2);

        glfwSwapBuffers(window);
        glfwPollEvents();

        glCheckError();
    }

    glfwTerminate();
    return 0;
}
