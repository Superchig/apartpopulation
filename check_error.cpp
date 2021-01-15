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

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                            GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 
    
    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;
    
    switch (source)
    {
        case GL_DEBUG_SOURCE_API_ARB:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER_ARB:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;
    
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR_ARB:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY_ARB:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:         std::cout << "Type: Performance"; break;
        // case GL_DEBUG_TYPE_MARKER_ARB:              std::cout << "Type: Marker"; break;
        // case GL_DEBUG_TYPE_PUSH_GROUP_ARB:          std::cout << "Type: Push Group"; break;
        // case GL_DEBUG_TYPE_POP_GROUP_ARB:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER_ARB:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH_ARB:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW_ARB:          std::cout << "Severity: low"; break;
        // case GL_DEBUG_SEVERITY_NOTIFICATION_ARB: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}
