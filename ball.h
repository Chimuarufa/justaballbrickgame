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

    // MODIFIED: Update now returns an INT (0 for no bricks hit, 1 for a brick hit)
    int Update(Paddle &paddle, std::vector<Brick> &bricks);
    void Draw();

private:
    void Move();

    // MODIFIED: Bounce also returns an INT
    int Bounce(Paddle &paddle, std::vector<Brick> &bricks);
};