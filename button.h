#ifndef BUTTON_H
#define BUTTON_H

#include "texture_2d.h"
#include "sprite_renderer.h"

class Button
{
  public:
    // Represents the center of the button
    float x;
    float y;
    int width;
    int height;

    Texture2D *texture;
    SpriteRenderer *spriteRenderer;

    Button(float x, float y, int width, int height, Texture2D *texture,
                   SpriteRenderer *spriteRenderer);

    void draw();
    bool hasInBounds(float x, float y);
};

#endif
