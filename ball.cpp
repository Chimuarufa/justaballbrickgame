#include "ball.h"
#include <math.h> // For fabs()

Ball::Ball(float posX, float posY, float r, float spX, float spY, Color c)
{
    x = posX;
    y = posY;
    radius = r;
    speedX = spX;
    speedY = spY;
    color = c;
    isAlive = true;
}
void Ball::Move()
{
    x += speedX;
    y += speedY;
}

// MODIFIED: This function now needs the gameArea for bounds
int Ball::Bounce(Paddle &paddle, std::vector<Brick> &bricks, Rectangle gameArea)
{
    int scoreGained = 0; // <-- MODIFIED: Was bricksHit, now tracks score

    // --- Wall Bouncing (Now relative to gameArea) ---

    // Bounce off left/right walls
    if (x + radius >= gameArea.x + gameArea.width || x - radius <= gameArea.x)
    {
        speedX = -speedX;
    }

    // Bounce off *top* wall only
    if (y - radius <= gameArea.y)
    {
        speedY = -speedY;
    }

    // --- Bottom Screen "Explosion" ---
    if (y + radius >= gameArea.y + gameArea.height)
    {
        isAlive = false;
    }

    // --- Paddle Bouncing ---
    if (CheckCollisionCircleRec({x, y}, radius, paddle.GetRect()))
    {
        if (speedY > 0) // Only bounce if ball is moving down
        {
            // 1. Always bounce up
            speedY = -fabs(speedY);

            // 2. Calculate *where* on the paddle it hit
            float hitPosition = (x - (paddle.x + paddle.width / 2)) / (paddle.width / 2);

            // 3. Change X speed based on hit position
            // We use 7.0f as a max horizontal speed factor
            speedX = hitPosition * 7.0f;
        }
    }

    // --- BRICK Bouncing (MODIFIED) ---
    for (Brick &brick : bricks)
    {
        if (brick.isAlive && CheckCollisionCircleRec({x, y}, radius, brick.GetRect()))
        {
            // --- NEW HEALTH LOGIC ---
            brick.health--;

            // Change color based on new health
            if (brick.health == 2)
            {
                brick.color = ORANGE;
            }
            else if (brick.health == 1)
            {
                brick.color = RED;
            }
            else if (brick.health <= 0)
            {
                brick.isAlive = false;
                scoreGained += brick.scoreValue; // <-- ADD SCORE!
            }

            // --- Bounce Logic ---
            // Figure out if we hit the side or top/bottom
            // This is a simple (but not perfect) way to check

            // Store old position to see where we came from
            float oldX = x - speedX;
            float oldY = y - speedY;

            // Check horizontal collision (hit the side of the brick)
            bool hitSide = (oldX + radius <= brick.x || oldX - radius >= brick.x + brick.width);

            if (hitSide)
            {
                speedX = -speedX;
            }
            else // Hit top or bottom
            {
                speedY = -speedY;
            }

            // We only handle one brick collision per frame for simplicity
            break;
        }
    }

    return scoreGained; // <-- MODIFIED: Return the score
}

// MODIFIED: Update function signature changed
int Ball::Update(Paddle &paddle, std::vector<Brick> &bricks, Rectangle gameArea)
{
    Move();
    int scoreGained = Bounce(paddle, bricks, gameArea); // Pass all info down
    return scoreGained;                                 // <-- MODIFIED: Pass the score back up
}

void Ball::Draw()
{
    if (isAlive)
    {
        DrawCircle((int)x, (int)y, radius, color);
    }
}