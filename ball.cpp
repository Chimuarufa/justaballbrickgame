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
    isAlive = true; // <-- ADDED: All balls start alive
}
void Ball::Move()
{
    x += speedX;
    y += speedY;
}

// MODIFIED: This function now returns an int
int Ball::Bounce(Paddle &paddle, std::vector<Brick> &bricks)
{
    int bricksHit = 0; // <-- ADDED: Counter to return

    // --- Wall Bouncing ---

    // Bounce off left/right walls
    if (x + radius >= GetScreenWidth() || x - radius <= 0)
    {
        speedX = -speedX;
    }

    // Bounce off *top* wall only
    if (y - radius <= 0)
    {
        speedY = -speedY;
    }

    // --- Bottom Screen "Explosion" ---
    // If ball hits bottom, mark it as dead
    if (y + radius >= GetScreenHeight())
    {
        isAlive = false; // <-- MODIFIED: No bounce, just "die"
    }

    // --- Paddle Bouncing ---
    if (CheckCollisionCircleRec({x, y}, radius, paddle.GetRect()))
    {
        // --- This is the new "advanced bounce" logic ---

        // 1. Always bounce up
        // We use -fabs() (negative absolute value) to guarantee
        // it *always* bounces UP, even if it gets stuck for a frame.
        speedY = -fabs(speedY);

        // 2. Calculate *where* on the paddle it hit
        //    paddle.x + paddle.width / 2      <-- This is the center X of the paddle
        //    x - (paddle_center)              <-- This is the distance from the center
        //    (distance) / (paddle.width / 2)  <-- This normalizes it to -1.0 to 1.0
        float hitPosition = (x - (paddle.x + paddle.width / 2)) / (paddle.width / 2);

        // 3. Change X speed based on hit position
        //    The "10.0f" is a "max horizontal speed" factor.
        //    If it hits the far left (-1.0), speedX becomes -10.0
        //    If it hits the center (0.0), speedX becomes 0.0
        //    If it hits the far right (1.0), speedX becomes 10.0
        speedX = hitPosition * 10.0f;
    }

    // --- 3. ADDED: BRICK BOUNCING ---
    for (Brick &brick : bricks)
    {
        // Only check for collision if the brick is alive
        if (brick.isAlive && CheckCollisionCircleRec({x, y}, radius, brick.GetRect()))
        {
            brick.isAlive = false; // "Kill" the brick
            bricksHit = 1;         // <-- ADDED: Report that we hit a brick

            // --- Collision Response Logic ---
            // Figure out if it was a vertical or horizontal hit.
            // This is a common AABB-style response.

            // Find the vector from ball center to brick center
            float brickCenterX = brick.x + brick.width / 2;
            float brickCenterY = brick.y + brick.height / 2;

            float vecX = x - brickCenterX;
            float vecY = y - brickCenterY;

            // Find how far into the brick the ball is, in each axis
            float overlapX = (radius + brick.width / 2) - fabs(vecX);
            float overlapY = (radius + brick.height / 2) - fabs(vecY);

            // If overlapX is smaller, it's a side collision
            if (overlapX < overlapY)
            {
                speedX = -speedX;
                // Nudge the ball out to prevent it from getting stuck
                x += (vecX > 0 ? overlapX : -overlapX);
            }
            // Otherwise, it's a top/bottom collision
            else
            {
                speedY = -speedY;
                // Nudge the ball out
                y += (vecY > 0 ? overlapY : -overlapY);
            }

            break; // Exit the loop immediately after hitting one brick
        }
    }

    return bricksHit; // <-- ADDED: Return the count
}

// Update function signature changed
int Ball::Update(Paddle &paddle, std::vector<Brick> &bricks)
{
    Move();
    int bricksHit = Bounce(paddle, bricks); // Pass the paddle AND bricks down
    return bricksHit;                       // <-- ADDED: Pass the count back up
}

void Ball::Draw()
{
    // Only draw if the ball is alive
    if (isAlive)
    {
        DrawCircle((int)x, (int)y, radius, color);
    }
}