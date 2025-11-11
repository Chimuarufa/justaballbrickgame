#pragma once
#include "raylib.h"

class Paddle
{
public:
    float x, y;
    float width, height;
    float speed;
    Color color;

    Paddle(float posX, float posY, float w, float h, float sp, Color c);

    // --- MODIFIED: Needs gameArea for clamping ---
    void Update(Rectangle gameArea);
    void Draw();

    // Helper function to easily get the paddle's rectangle shape
    // for collision checks
    Rectangle GetRect();
};