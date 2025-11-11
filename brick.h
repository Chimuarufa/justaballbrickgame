#pragma once
#include "raylib.h"

class Brick
{
public:
    float x, y;
    float width, height;
    Color color;
    bool isAlive;
    int health;
    int scoreValue;

    Brick(float posX, float posY, float w, float h, int hp, int score, Color c);

    void Draw();

    // --- ADDED: The missing helper function ---
    Rectangle GetRect();
};