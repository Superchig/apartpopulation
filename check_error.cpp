#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "check_error.h"

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        // The commented-out flags aren't supported by OpenGL 3.3
        // case GL_STACK_OVERFLOW:
        //     error = "STACK_OVERFLOW";
        //     break;
        // case GL_STACK_UNDERFLOW:
        //     error = "STACK_UNDERFLOW";
        //     break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
