#ifndef GAME_H
#define GAME_H

#include <glm/glm.hpp>

class Game
{
  public:
    float eyeX;
    float eyeY;
    float eyeZ;

    glm::mat4 projection;
};

#endif
