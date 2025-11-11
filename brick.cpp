#include "Brick.h"

Brick::Brick(float posX, float posY, float w, float h, int hp, int score, Color c)
{
    x = posX;
    y = posY;
    width = w;
    height = h;
    isAlive = true;
    health = hp;        // <-- ADDED
    scoreValue = score; // <-- ADDED
    color = c;
}

void Brick::Draw()
{
    if (isAlive)
    {
        // Draw the brick
        DrawRectangle((int)x, (int)y, (int)width, (int)height, color);
        // Add a little 3D-ish border
        DrawRectangleLines((int)x, (int)y, (int)width, (int)height, BLACK);
    }
}

// --- ADDED: The definition for the new function ---
Rectangle Brick::GetRect()
{
    // raylib's Rectangle struct: { x, y, width, height }
    return {x, y, width, height};
}