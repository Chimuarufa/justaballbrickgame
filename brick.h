#pragma once
#include "raylib.h"

class Brick
{
public:
    float x, y;
    float width, height;
    bool isAlive;
    Color color;

    // Constructor to set up a new brick
    Brick(float posX, float posY, float w, float h, Color c);

    void Draw();

    // Helper function just like the paddle's
    Rectangle GetRect();
};