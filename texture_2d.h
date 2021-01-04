#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture2D
{
  public:
    GLuint       ID;
    unsigned int width;
    unsigned int height;
    GLuint       internalFormat;
    GLuint       imageFormat;
    GLuint       wrapS;
    GLuint       wrapT;
    GLuint       filterMin;
    GLuint       filterMax;

    Texture2D(const char *file, bool alpha, int filter = GL_LINEAR);

    void bind() const;
};

#endif
