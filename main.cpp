#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

float vertices[] = {
     0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // Top
    -1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // Bottom left
     1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // Bottom right
};

// void drawTriangle(glm::vec2 position)
// {
//     glm::mat4 model = glm::mat4(1.0f);
//     model = glm::translate(model, glm::vec3(position, 0.0f));
// }

int main()
{
    std::cout << "Hello, world!" << std::endl;
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    
    Shader shaderProgram("shader.vs", "shader.fs");
    
    float vertices[] = {
         0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // Top
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // Bottom left
         1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // Bottom right
    };
    
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    GLint modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
    
    shaderProgram.use();
    
    // glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -3.5f, 3.5f);
    // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f); 
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    // projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -3.5f, 3.5f) * projection;
    // glm::mat4 projScale = glm::scale(glm::mat4(1.0f), glm::vec3(400.0f, 300.0f, 1.0f));
    // projection = projection * projScale;
    // projection = projScale * projScale;
    
    GLint projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    float xOffset = 0.0f;
    float yOffset = 0.0f;
    float zOffset = -3.0f;
    
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
        {
            zOffset += 0.01f;
            std::cout << "zDist: " << zOffset << std::endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            zOffset -= 0.01f;
            std::cout << "zDist: " << zOffset << std::endl;
        }
        
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            yOffset -= 1.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            yOffset += 1.0f;
        }
        
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            xOffset += 1.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            xOffset -= 1.0f;
        }

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        shaderProgram.use();
        
        // glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(xOffset, yOffset, zOffset));
        // glm::mat4 view = glm::mat4(1.0f);
        // glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        glm::vec3 eye = glm::vec3(-xOffset, -yOffset, -zOffset);
        glm::vec3 center = eye + glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(eye, center, up);
        GLint viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(400.0f, 300.0f, 0.0f));
        
        // model = glm::rotate(model, 3.0f * glm::radians((float) glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // The triangle is rotated on the x-axis a bit!
        
        // model = glm::scale(model, glm::vec3(400.0f, 300.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
