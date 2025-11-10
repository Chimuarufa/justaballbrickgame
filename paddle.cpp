#include "Paddle.h"

Paddle::Paddle(float posX, float posY, float w, float h, float sp, Color c)
{
    x = posX;
    y = posY;
    width = w;
    height = h;
    speed = sp;
    color = c;
}

void Paddle::Update()
{
    // --- Mouse Control (Active) ---
    // This is the one we talked about that feels more natural
    x = GetMouseX() - (width / 2); // Center the paddle on the mouse

    // --- Keyboard Control (Commented out) ---
    // If you want to use keyboard, just uncomment this block
    // and comment out the "Mouse Control" line above.
    /*
    if (IsKeyDown(KEY_LEFT))
    {
        x -= speed;
    }
    if (IsKeyDown(KEY_RIGHT))
    {
        x += speed;
    }
    */

    // --- Screen Clamping (so it doesn't go off-screen) ---
    if (x <= 0)
    {
        x = 0;
    }
    if (x + width >= GetScreenWidth())
    {
        x = GetScreenWidth() - width;
    }
}

void Paddle::Draw()
{
    DrawRectangle((int)x, (int)y, (int)width, (int)height, color);
}

// Helper function definition
Rectangle Paddle::GetRect()
{
    // raylib's Rectangle struct: { x, y, width, height }
    return {x, y, width, height};
}