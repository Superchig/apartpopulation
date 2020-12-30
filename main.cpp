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

float eyeChange(float eyeZ) { return 10.0f * Game::main.zoomFactor; }

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
    
    shader.setMatrix("projection", Game::main.projection);

    // glm::mat4 view   = glm::lookAt(eye, center, up);
    shader.setMatrix("view", Game::main.view);

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
    
    // Display the top-right coordinate homogeneous clip space
    // -------------------------------------------------------
    // glm::mat4 MVP = Game::main.projection * Game::main.view * model;
    // glm::vec4 topRight = MVP * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
    // glm::vec4 topRightPersp = topRight / topRight.w;
    // std::cout << "topRight: " << topRight.x << ", " << topRight.y << ", " << topRight.z << ", " << topRight.w << std::endl;
    // std::cout << "topRightPersp: " << topRightPersp.x << ", " << topRightPersp.y << ", " << topRightPersp.z << ", " << topRightPersp.w << std::endl;
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
    // Game::main.projection = glm::perspective(
    //     glm::radians(45.0f), (float)Game::main.window_width / (float)Game::main.window_height, 0.1f,
    //     1500.0f);
    // Game::main.projection = glm::ortho(-640.0f, 640.0f, -360.0f, 360.0f, 0.1f, 1500.0f);
    Game::main.projection = glm::ortho(-640.0f * Game::main.zoomFactor, 640.0f * Game::main.zoomFactor,
                                       -360.0f * Game::main.zoomFactor, 360.0f * Game::main.zoomFactor,
                                       0.1f, 1500.0f);
    GLint projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(Game::main.projection));

    // Declare camera manipulation variables
    // -------------------------------------
    const float zChange = 10.0f;

    // Initialize and set up table and text
    // ------------------------------------
    Shader       textShader("text.vert", "text.frag");
    TextRenderer textRen("fonts/Cantarell-Regular.otf", &textShader, 32);
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
            // Game::main.eyeZ -= zChange;
            // std::cout << "Game::main.eyeZ: " << Game::main.eyeZ << std::endl;
            Game::main.zoomFactor -= 0.01f;
            
            Game::main.updateOrtho();
        }
        else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            // Game::main.Game::main.eyeZ += zChange;
            // std::cout << "Game::main.eyeZ: " << Game::main.eyeZ << std::endl;
            Game::main.zoomFactor += 0.01f;
            
            Game::main.updateOrtho();
        }
        else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            Game::main.zoomFactor -= 1.0f;
            Game::main.eyeX = 0.0f;
            Game::main.eyeY = 0.0f;
            
            Game::main.updateOrtho();
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

        double xPos;
        double yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        std::stringstream sstream;
        sstream << "Mouse cursor: " << xPos << ", " << yPos;
        textRen.renderText(sstream.str(), -640.0f, -32.0f, 1.0f,
                           glm::vec3(1.0f, 1.0f, 1.0f));
        
        // Coordinates in clip space for mouse cursor.
        // Note: With the view and perspective
        // projection matrix I have set up, the w value seems to roughly equal the
        // distance between the eye and the xy-plane (eye's z-value). The w value
        // is a little higher than the z value for homogeneous clip space coordinates,
        // but I don't understand the math well enough to know why (the absolute
        // difference between the values changes as they get larger, but I don't know
        // why that happens either).
        const double xNDC = (xPos / (Game::main.window_width / 2.0f)) - 1.0f;
        const double yNDC = 1.0f - (yPos / (Game::main.window_height / 2.0f));
        sstream.str(std::string());
        sstream << "Mouse NDC: " << xNDC << ", " << yNDC;
        textRen.renderText(sstream.str(), -640.0f, 0.0f, 1.0f,
                           glm::vec3(1.0f, 1.0f, 1.0f));
        
        // drawTriangle(shaderProgram, 0.5f, 0.0f, 1.0f, 0.0f);
        // Note: 1473 and 829 are the width and height in (z = 0)
        // world space when eye is at z = 1000
        const float rectX = -500.0f;
        const float rectY = 200.0f;
        const float rectWidth = 200.0f;
        const float rectHeight = 200.0f;
        drawRectangle(shaderProgram, rectX, rectY, rectWidth, rectHeight, 0.0f);
        glm::vec3 topRightWorld = glm::vec3(rectX + (rectWidth / 2.0f), rectY + (rectHeight / 2.0f), 0.0f);
        std::cout << "topRightWorld: " << topRightWorld.x << ", " << topRightWorld.y << ", " << topRightWorld.z << std::endl;
        
        // This math doesn't work with perspective projection matrices, but it does
        // work with an orthographic projection matrix
        glm::mat4 VP = Game::main.projection * Game::main.view;
        glm::mat4 VPinv = glm::inverse(VP);
        glm::vec4 mouseClip = glm::vec4((float)xNDC, (float)yNDC, 1.0f, 1.0f);
        glm::vec4 worldMouse = VPinv * mouseClip;
        std::cout << "worldMouse: " << worldMouse.x << ", " << worldMouse.y << ", " << worldMouse.z << ", " << worldMouse.w << std::endl;
        
        spreadTable.draw();

        lineRen.drawLine(0.0f, 0.0f, 0.0f, 300.0f);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        glCheckError();
    }

    glfwTerminate();
    return 0;
}
