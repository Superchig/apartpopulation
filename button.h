#ifndef BUTTON_H
#define BUTTON_H

#include "texture_2d.h"

#include <functional>
#include "text_renderer.h"

class Button
{
  public:
    // Represents the center of the button
    float x;
    float y;
    int width;
    int height;

    Texture2D *texture;
    TextRenderer *textRen;
    
    std::string text;
    float vertPadding;
    
    // TODO: Clean up button callback syntax
    std::function<void(Button *button)> callback;

    Button(float x, float y, int width, int height, Texture2D *texture,
           TextRenderer *textRen, std::string text = std::string(),
           float vertPadding = 0.0f);

    void draw();
    bool hasInBounds(float x, float y);
};

#endif
