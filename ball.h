#pragma once
#include "raylib.h"
#include "Paddle.h"
#include "Brick.h" // <-- 1. ADDED: Include the new Brick class
#include <vector>  // <-- 2. ADDED: So we can pass the vector of bricks

class Ball
{
public:
    float x, y;
    float speedX, speedY;
    float radius;
    Color color;
    bool isAlive; // <-- ADDED: To track if it's in play

    Ball(float posX, float posY, float r, float spX, float spY, Color c);

    // MODIFIED: Update now needs the gameArea
    int Update(Paddle &paddle, std::vector<Brick> &bricks, Rectangle gameArea);
    void Draw();

private:
    void Move();

    // MODIFIED: Bounce also needs the gameArea
    int Bounce(Paddle &paddle, std::vector<Brick> &bricks, Rectangle gameArea);
};