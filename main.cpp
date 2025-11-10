#include "raylib.h"
#include "ball.h"
#include "Paddle.h"
#include "Brick.h" // <-- 1. INCLUDE THE NEW BRICK CLASS
#include <vector>
#include <iostream>
#include <string> // <-- ADDED: Need this to draw text

// --- 2. ADDED: A FUNCTION TO CREATE OUR RANDOM LEVEL ---
void CreateLevel(std::vector<Brick> &bricks, int screenWidth)
{
    bricks.clear(); // Clear out any old bricks

    // Define the layout properties
    const int rowCount = 5;
    const int colCount = 10;
    const float brickWidth = 60.0f;
    const float brickHeight = 20.0f;
    const float padding = 5.0f;

    // Calculate the total width of the block of bricks
    float totalBlockWidth = (colCount * brickWidth) + ((colCount - 1) * padding);
    // Calculate the X offset to center the block
    float offsetX = (screenWidth - totalBlockWidth) / 2.0f;
    float offsetY = 50.0f; // Start 50px from the top

    for (int r = 0; r < rowCount; r++)
    {
        for (int c = 0; c < colCount; c++)
        {
            // --- THIS IS THE RANDOM PART ---
            // 50/50 chance to spawn a brick
            if (GetRandomValue(0, 1) == 1)
            {
                float brickX = offsetX + c * (brickWidth + padding);
                float brickY = offsetY + r * (brickHeight + padding);

                // Give it a random color
                Color color = Color{(unsigned char)GetRandomValue(50, 220),
                                    (unsigned char)GetRandomValue(50, 220),
                                    (unsigned char)GetRandomValue(50, 220), 255};

                bricks.push_back(Brick(brickX, brickY, brickWidth, brickHeight, color));
            }
        }
    }
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Simple Ball Animation");

    // Vector for mah bawls
    std::vector<Ball> balls;

    // --- 3. ADDED: Vector for the bricks ---
    std::vector<Brick> bricks;

    // --- 4. ADDED: Create the first random level ---
    CreateLevel(bricks, screenWidth);

    // --- CREATE THE PADDLE ---
    // --- 2. CREATE THE PADDLE ---
    float paddleWidth = 100.0f;
    float paddleHeight = 20.0f;
    Paddle paddle(
        (screenWidth / 2) - (paddleWidth / 2), // Centered X
        screenHeight - paddleHeight - 20,      // At the bottom (20px padding)
        paddleWidth,
        paddleHeight,
        7.0f, // Paddle speed (for keyboard)
        BLACK);

    // --- ADDED: Mad Scientist Mode Toggle ---
    bool isChaosMode = false; // Start in normal mode

    // --- ADDED: Your Wild Idea Counters ---
    int bricksDestroyed = 0;
    int ballsDestroyed = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // --- 1. UPDATE ---

        // Update the paddle (mouse or keyboard)
        paddle.Update();

        // --- ADDED: Check for mode toggle ---
        if (IsKeyPressed(KEY_C))
        {
            isChaosMode = !isChaosMode; // Flip the boolean
        }

        // --- Ball Spawning Logic (Now with 100% more madness) ---

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (isChaosMode)
            {
                // --- Version 2: Chaos Mode (Active if isChaosMode is true) ---
                float newRadius = 10.0f;
                balls.push_back(Ball(
                    paddle.x + (paddle.width / 2),
                    paddle.y - newRadius,
                    newRadius,
                    (float)GetRandomValue(-8, 8), // Random X speed
                    -7.0f,                        // Start moving up
                    Color{(unsigned char)GetRandomValue(50, 220),
                          (unsigned char)GetRandomValue(50, 220),
                          (unsigned char)GetRandomValue(50, 220), 255} // Random color
                    ));
            }
            else
            {
                // --- Version 1: One Ball at a Time (Active if isChaosMode is false) ---
                if (balls.empty()) // Only spawn if no balls exist
                {
                    float newRadius = 10.0f;
                    balls.push_back(Ball(
                        paddle.x + (paddle.width / 2), // Spawn at paddle center X
                        paddle.y - newRadius,          // Spawn just above paddle
                        newRadius,
                        0.0f,  // Start with no X speed
                        -7.0f, // Start moving up
                        BLACK  // Consistent color
                        ));
                }
            }
        }

        // Update all balls (and pass the paddle AND bricks in)
        for (Ball &ball : balls)
        {
            if (ball.isAlive)
            {
                // MODIFIED: Add the returned value (0 or 1) to our counter
                bricksDestroyed += ball.Update(paddle, bricks);
            }
        }

        // --- 4. "EXPLODE" (Remove) Dead Balls ---
        // We MUST loop backwards when removing items from a vector
        // to avoid skipping elements.
        for (int i = balls.size() - 1; i >= 0; i--)
        {
            if (!balls[i].isAlive)
            {
                ballsDestroyed++; // <-- ADDED: Count the destroyed ball
                balls.erase(balls.begin() + i);
                // std::cout << "A ball has been removed!" << std::endl;
            }
        }

        // --- 6. ADDED: Check for win condition ---
        bool allBricksDead = true;
        for (Brick &brick : bricks)
        {
            if (brick.isAlive)
            {
                allBricksDead = false;
                break;
            }
        }

        // If all bricks are dead, create a new random level!
        if (allBricksDead)
        {
            CreateLevel(bricks, screenWidth);
            // --- ADDED: Reset scores for the new level! ---
            bricksDestroyed = 0;
            ballsDestroyed = 0;
        }

        // --- 2. DRAWING ---
        BeginDrawing();
        ClearBackground(WHITE);

        // --- MODIFIED: DrawText ---
        // DrawText("Look! They bounce!", 10, 10, 20, BLACK);

        // --- ADDED: UI Text for our new mode ---
        std::string modeText = "Mode: ";
        Color modeColor = BLACK;
        if (isChaosMode)
        {
            modeText += "CHAOS (Press C)";
            modeColor = RED;
        }
        else
        {
            modeText += "Normal (Press C)";
            modeColor = BLACK;
        }
        DrawText(modeText.c_str(), 10, 10, 20, modeColor);

        // --- ADDED: Draw the new stats ---
        std::string brickText = "Bricks: " + std::to_string(bricksDestroyed);
        std::string ballText = "Balls Lost: " + std::to_string(ballsDestroyed);
        DrawText(brickText.c_str(), 10, 40, 20, DARKGREEN);
        DrawText(ballText.c_str(), 10, 70, 20, MAROON);

        // --- ADDED: The Cheeky Ratio Bar ---
        float barWidth = 200.0f;
        float barHeight = 20.0f;
        float barX = screenWidth - barWidth - 10; // Top right corner
        float barY = 10.0f;
        int totalDestroyed = bricksDestroyed + ballsDestroyed;

        float brickRatio = 0.0f;
        if (totalDestroyed > 0)
        {
            // This is the percentage of the bar that bricks take up
            brickRatio = (float)bricksDestroyed / (float)totalDestroyed;
        }

        // Draw the "empty" part of the bar (representing balls)
        DrawRectangle(barX, barY, barWidth, barHeight, Fade(MAROON, 0.5f));
        // Draw the "full" part of the bar (representing bricks)
        DrawRectangle(barX, barY, (int)(barWidth * brickRatio), barHeight, DARKGREEN);
        // Draw an outline
        DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, BLACK);

        // Draw a label
        DrawText("Brick / Ball Ratio", (int)barX, (int)barY + (int)barHeight + 5, 10, BLACK);

        // Draw all the bricks
        for (Brick &brick : bricks)
        {
            brick.Draw();
        }

        // Draw all balls that are still in the vector
        for (Ball &ball : balls)
        {
            ball.Draw();
        }

        // Draw the paddle
        paddle.Draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}