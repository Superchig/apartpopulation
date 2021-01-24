#include "texture_2d.h"
#include "stb_image.h"

#include <iostream>
#include <climits>

Texture2D::Texture2D(const char *file, bool alpha, int filter)
    : width(0), height(0), internalFormat(GL_RGB), imageFormat(GL_RGB), wrapS(GL_REPEAT),
      wrapT(GL_REPEAT), filterMin(filter), filterMax(filter)
{
    glGenTextures(1, &this->ID);
    std::cout << "Texture ID from " << file << ": " << this->ID << std::endl;
    
    if (alpha)
    {
        this->internalFormat = GL_RGBA;
        this->imageFormat = GL_RGBA;
    }
    
    stbi_set_flip_vertically_on_load(true);

    int imageWidth;
    int imageHeight;
    int nrChannels;
    unsigned char *data = stbi_load(file, &imageWidth, &imageHeight, &nrChannels, 0);
    
    this->width = imageWidth;
    this->height = imageHeight;
    
    // Create texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, imageWidth, imageHeight, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);
    // Set texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMax);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    
    stbi_image_free(data);
}

Texture2D::Texture2D()
    : width(1), height(1), internalFormat(GL_RGB), imageFormat(GL_RGB), wrapS(GL_REPEAT),
      wrapT(GL_REPEAT), filterMin(GL_LINEAR), filterMax(GL_LINEAR)
{
    glGenTextures(1, &this->ID);
    
    constexpr unsigned char data[] = {UCHAR_MAX, UCHAR_MAX, UCHAR_MAX};
    
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::bind() const { glBindTexture(GL_TEXTURE_2D, this->ID); }

Texture2D *Texture2D::newWhiteTexture()
{
    return new Texture2D();
}
