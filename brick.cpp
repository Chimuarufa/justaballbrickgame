#include "brick.h"

// Constructor: Just assigns all the values
Brick::Brick(float posX, float posY, float w, float h, Color c)
{
    x = posX;
    y = posY;
    width = w;
    height = h;
    color = c;
    isAlive = true; // All bricks start alive
}

void Brick::Draw()
{
    // Only draw the brick if it's alive
    if (isAlive)
    {
        DrawRectangle((int)x, (int)y, (int)width, (int)height, color);
    }
}

Rectangle Brick::GetRect()
{
    return {x, y, width, height};
}