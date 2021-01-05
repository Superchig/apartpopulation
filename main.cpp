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
#include "texture_2d.h"
#include "sprite_renderer.h"
#include "button.h"
#include "historical_figure.h"

Game Game::main;

float eyeChange(float eyeZ) { return 10.0f * Game::main.zoomFactor; }

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

void advanceMonthCallback(Button *button);
void syncPopTable(Table &popTable);

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

    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Set projection matrix outside of render loop
    // -----------------------------------
    Game::main.projection = glm::ortho(-640.0f * Game::main.zoomFactor, 640.0f * Game::main.zoomFactor,
                                       -360.0f * Game::main.zoomFactor, 360.0f * Game::main.zoomFactor,
                                       0.1f, 1500.0f);

    // Declare camera manipulation variables
    // -------------------------------------
    const float zChange = 10.0f;

    // Load shaders and other resources
    // --------------------------------
    Texture2D container{"sprites/container.jpg", false};
    Texture2D buttonNormal{"sprites/adwitr-5th-button0.png", true, GL_NEAREST};
    // Texture2D buttonPressed{"sprites/adwitr-5th-button1", true};

    Shader       textShader("shaders/text.vert", "shaders/text.frag");
    TextRenderer textRen("fonts/Cantarell-Regular.otf", &textShader, 32);

    glCheckError();

    Shader       shader2d("shaders/2d_shader.vert", "shaders/2d_shader.frag");
    LineRenderer lineRen(&shader2d);

    glCheckError();

    Shader         spriteShader{"shaders/sprite.vert", "shaders/sprite.frag"};
    SpriteRenderer spriteRen{spriteShader};

    glCheckError();

    Button button{-0.25f * Game::main.window_width,
                  -0.25f * Game::main.window_height,
                  34 * 10,
                  10 * 10,
                  &buttonNormal,
                  &spriteRen,
                  &textRen,
                  "Pass Month",
                  34 * 10 * 0.25f};
    button.callback = advanceMonthCallback;
    Game::main.buttons.push_back(&button);
    
    // Set up initial noble population
    // -------------------------------
    for (int i = 0; i < 5; i++)
    {
        Game::main.livingFigures.push_back(HistoricalFigure());
    }

    // Initialize and set up table
    // ---------------------------
    Table spreadTable(-200.0f, 300.0f, Game::main.livingFigures.size() + 1,
                                   &textRen, &lineRen);
    Game::main.spreadTable = &spreadTable;
    spreadTable.setColWidth(0, 400);
    spreadTable.setColWidth(1, 200);
    spreadTable.setItem(0, 0, "Name");
    spreadTable.setItem(0, 1, "Age");
    
    syncPopTable(spreadTable);

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
            Game::main.zoomFactor = 1.0f;
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
        // sstream << "Mouse cursor: " << xPos << ", " << yPos;
        // textRen.renderText(sstream.str(), -640.0f, -32.0f, 1.0f,
        //                    glm::vec3(1.0f, 1.0f, 1.0f));
        
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
        
        const float rectX = -500.0f;
        const float rectY = 200.0f;
        const float rectWidth = 200.0f;
        const float rectHeight = 200.0f;
        // drawRectangle(shaderProgram, rectX, rectY, rectWidth, rectHeight, 0.0f);
        glm::vec3 topRightWorld = glm::vec3(rectX + (rectWidth / 2.0f), rectY + (rectHeight / 2.0f), 0.0f);
        // std::cout << "topRightWorld: " << topRightWorld.x << ", " << topRightWorld.y << ", " << topRightWorld.z << std::endl;
        sstream.str("");
        sstream << "topRightWorld: " << topRightWorld.x << ", " << topRightWorld.y << ", " << topRightWorld.z;
        textRen.renderText(sstream.str(), -640.0f, 0.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        // This math doesn't work with perspective projection matrices, but it does
        // work with an orthographic projection matrix
        glm::mat4 VP = Game::main.projection * Game::main.view;
        glm::mat4 VPinv = glm::inverse(VP);
        glm::vec4 mouseClip = glm::vec4((float)xNDC, (float)yNDC, 1.0f, 1.0f);
        glm::vec4 worldMouse = VPinv * mouseClip;
        // std::cout << "worldMouse: " << worldMouse.x << ", " << worldMouse.y << ", " << worldMouse.z << ", " << worldMouse.w << std::endl;
        sstream.str("");
        sstream << "worldMouse: " << worldMouse.x << ", " << worldMouse.y << ", " << worldMouse.z << ", " << worldMouse.w;
        textRen.renderText(sstream.str(), -640.0f, -32.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        Game::main.mouseX = worldMouse.x;
        Game::main.mouseY = worldMouse.y;
        
        // The previous month has already happened and this month is yet to happen.
        const std::string date = "Year: " + std::to_string(Game::main.date.year)
            + ", month: " + std::to_string(Game::main.date.month);
        textRen.renderText(date, -640.0f, Game::main.window_height / 2.0f - 32.0f * 3.0f,
                           1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        spriteRen.drawSprite(container, glm::vec2(Game::main.playerX, Game::main.playerY),
                             glm::vec2(200.0f, 200.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        
        spreadTable.draw();
        button.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        glCheckError();
    }

    glfwTerminate();
    return 0;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        for (Button *button : Game::main.buttons)
        {
            if (button->hasInBounds(Game::main.mouseX, Game::main.mouseY))
            {
                // std::cout << "Left mouse pressed a BUTTON at " << Game::main.mouseX
                //           << ", " << Game::main.mouseY << std::endl;
                
                button->callback(button);
                break;
            }
        }
    }
}

void advanceMonthCallback(Button *button)
{
    // Advance the simulation
    // ----------------------
    for (HistoricalFigure &figure : Game::main.livingFigures)
    {
        // std::cout << figure.name << "'s birth day: year " << figure.birthDay.year << ", month " << figure.birthDay.month << std::endl;
        
        if (figure.birthDay.month == Game::main.date.month && figure.birthDay.year != Game::main.date.year)
        {
            figure.age++;
            std::cout << figure.name << " celebrates their birthday, turning "
                      << figure.age << "." << std::endl;
        }
    }
    
    // Update table UI
    // ---------------
    syncPopTable(*Game::main.spreadTable);
    
    // Update the actual month
    // -----------------------
    if (Game::main.date.month >= DEC)
    {
        Game::main.date.month = JAN;
        Game::main.date.year++;
    }
    else
    {
        Game::main.date.month = (Month) (Game::main.date.month + 1);
    }
}

void syncPopTable(Table &popTable)
{
    for (int i = 0; i < Game::main.livingFigures.size(); i++)
    {
        const HistoricalFigure &figure = Game::main.livingFigures[i];
        
        popTable.setItem(i + 1, 0, figure.name);
        popTable.setItem(i + 1, 1, std::to_string(figure.age));
    }
}
