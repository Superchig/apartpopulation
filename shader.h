#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // Include glad to get all the required OpenGL headers
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // The program ID
    GLuint ID;
    
    // Constructor reads and builds the shader
    Shader(const char *vertexPath, const char *fragmentPath);
    // Use/activate the shader
    void use();
    // Utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMatrix(const std::string &name, const glm::mat4 &value) const;
};

#endif
