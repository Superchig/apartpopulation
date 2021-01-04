#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <glad/glad.h>
#include <map>
#include <string>
#include "texture_2d.h"
#include "shader.h"

class ResourceManager
{
  public:
      // Resource storage
    static std::map<std::string, Shader> shaders;
    static std::map<std::string, Texture2D> textures;
    static Shader                           loadShader(const char *vShaderFile, const char *fShaderFile, std::string name);
};

#endif