#include <iostream>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader.h"

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

struct Character {
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    FT_Pos Advance;
};

GLuint VAO_FONT;
GLuint VBO_FONT;
GLuint VAO_TRIG;
GLuint VBO_TRIG;
GLuint VAO_RECT;
GLuint VBO_RECT;
std::map<char, Character> Characters;
float eyeX = 0.0f;
float eyeY = 0.0f;
float eyeZ = 3.0f;
float playerX = 0.0f;
float playerY = 0.0f;
const float playerChange = 0.01f;

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

float eyeChange(float eyeZ) {
    return eyeZ / 100.0f;
}

void renderText(Shader &s, const std::string &text, float x, float y, float scale, glm::vec3 color)
{
    // Activate the corresponding render state
    s.use();
    glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_FONT);
    
    glm::mat4 model = glm::mat4(1.0f);
    s.setMatrix("model", model);
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f); 
    s.setMatrix("projection", projection);
    
    glm::vec3 eye = glm::vec3(eyeX, eyeY, eyeZ);
    glm::vec3 center = eye + glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(eye, center, up);
    s.setMatrix("view", view);
    
    // Iterate through all the characters
    for (char c : text)
    {
        Character ch = Characters[c];
        
        float xPos = x + ch.Bearing.x * scale;
        float yPos = y - (ch.Size.y - ch.Bearing.y) * scale;
        
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        
        // Update VBO for each character
        float vertices[6][4] = {
            {xPos, yPos + h, 0.0f, 0.0f },
            {xPos, yPos, 0.0f, 1.0f},
            {xPos + w, yPos, 1.0f, 1.0f},
            
            {xPos, yPos + h, 0.0f, 0.0f},
            {xPos + w, yPos, 1.0f, 1.0f},
            {xPos + w, yPos + h, 1.0f, 0.0f}
        };
        
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO_FONT);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2*6 = 64)
    }
    // glBindVertexArray(0);
    // glBindTexture(GL_TEXTURE_2D, 0);
}

void drawTriangle(Shader &shader, float centerX, float centerY, float scale, float rotateDeg)
{
    shader.use();

    glm::vec3 eye = glm::vec3(eyeX, eyeY, eyeZ);
    glm::vec3 center = eye + glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(eye, center, up);
    shader.setMatrix("view", view);
    
    // Draw triangle
    // -------------
    glBindVertexArray(VAO_TRIG);
    
    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, glm::vec3(centerX, centerY, 0.0f));
    model = glm::rotate(model, glm::radians(rotateDeg), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
    
    shader.setMatrix("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawRectangle(Shader &shader, float centerX, float centerY, float width, float height, float rotateDeg)
{
    shader.use();

    glm::vec3 eye = glm::vec3(eyeX, eyeY, eyeZ);
    glm::vec3 center = eye + glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(eye, center, up);
    shader.setMatrix("view", view);
    
    // Draw rectangle
    // --------------
    glBindVertexArray(VAO_RECT);
    
    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, glm::vec3(centerX, centerY, 0.0f));
    model = glm::rotate(model, glm::radians(rotateDeg), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    
    shader.setMatrix("model", model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main()
{
    std::cout << "Hello, world!" << std::endl;
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "OpenGL");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "OpenGL");
    
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Platformer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << '\n';
        return -1;
    }
    
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType library" << std::endl;
        return -1;
    }
    
    FT_Face face;
    if (FT_New_Face(ft, "fonts/Cantarell-Regular.otf", 0, &face))
    {
        std::cout  << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }
    
    FT_Set_Pixel_Sizes(face, 0, 48);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
    
    // Initialize Characters
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
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        
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
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    
    // Clear FreeType's resources now that we're done using them
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Shader shaderProgram("shader.vs", "shader.fs");
    Shader textShader("text.vs", "text.fs");
    
    const float SIDE_LENGTH = 1.0f;
    const float HALF_SIDE = SIDE_LENGTH / 2.0f;
    const float TRI_HEIGHT = SIDE_LENGTH * sqrtf(3.0f) / 2.0f;
    const float HALF_HEIGHT = TRI_HEIGHT / 2.0f;
    
    // Equilateral triangle
    float triVertices[] = {
        0.0f,       HALF_HEIGHT, 0.0f,  1.0f, 0.0f, 0.0f, // Top
        -HALF_SIDE, -HALF_HEIGHT, 0.0f,  0.0f, 1.0f, 0.0f, // Bottom left
        HALF_SIDE, -HALF_HEIGHT, 0.0f,  0.0f, 0.0f, 1.0f, // Bottom right
    };
    
    float rectVertices[] = {
        // Positions        // Colors
        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Top right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // Bottom left
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  // Top left
    };
    
    unsigned int rectIndices[] = {
        0, 1, 3,   // First triangle
        1, 2, 3,   // Second triangle
    };  
    
    glGenVertexArrays(1, &VAO_TRIG);
    glBindVertexArray(VAO_TRIG);
    
    glGenBuffers(1, &VBO_TRIG);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TRIG);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Initialize vertices for rectangle
    // ---------------------------------
    glGenVertexArrays(1, &VAO_RECT);
    glBindVertexArray(VAO_RECT);
    
    glGenBuffers(1, &VBO_RECT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_RECT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    GLuint EBO_RECT;
    glGenBuffers(1, &EBO_RECT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_RECT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIndices), rectIndices, GL_STATIC_DRAW);
    
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
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f); 
    GLint projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Declare camera manipulation variables
    // -------------------------------------
    const float zChange = 0.1f;
    
    glCheckError();
    
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
        {
            eyeZ -= zChange;
            std::cout << "eyeZ: " << eyeZ << std::endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            eyeZ += zChange;
            std::cout << "eyeZ: " << eyeZ << std::endl;
        }
        
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        {
            eyeY += eyeChange(eyeZ);
        }
        else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        {
            eyeY -= eyeChange(eyeZ);
        }
        
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        {
            eyeX -= eyeChange(eyeZ);
        }
        else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            eyeX += eyeChange(eyeZ);
        }
        
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            playerY += playerChange;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            playerY -= playerChange;
        }
        
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            playerX += playerChange;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            playerX -= playerChange;
        }
        
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        // glClearColor(0.0f, 0.19215686274509805f, 0.3254901960784314f, 0.0f); // Prussian blue
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawTriangle(shaderProgram, -0.5f, -0.5f, 0.5f, 30.0f * glfwGetTime());
        drawTriangle(shaderProgram, playerX, playerY, 0.5f, 90.0f);
        
        drawRectangle(shaderProgram, 0.0f, -0.25f, 1.0f, 0.25f, 0.0f);
        drawRectangle(shaderProgram, 0.0f, 0.5f, 1.0f, 0.25f, 30.0f * glfwGetTime());
        
        // Render text
        // -----------
        renderText(textShader, "Hello, world!", 0.0f, 0.0f, 0.005f, glm::vec3(0.5f, 0.8f, 0.2f));
        renderText(textShader, "Line two!", 0.0f, -0.3f, 0.005f, glm::vec3(0.5f, 0.8f, 0.2f));
        
        double xPos;
        double yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        std::stringstream sstream;
        sstream << "Mouse cursor: " << xPos << ", " << yPos;
        renderText(textShader, sstream.str(), -1.0f, -1.0f, 0.005f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
