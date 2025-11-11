#include "raylib.h"
#include "ball.h"
#include "Paddle.h"
#include "Brick.h"
#include <vector>
#include <iostream>
#include <string>

// --- A FUNCTION TO CREATE OUR RANDOM LEVEL ---
// Takes the vector of bricks (by reference) and the gameArea to calculate offsets
void CreateLevel(std::vector<Brick> &bricks, Rectangle gameArea)
{
    bricks.clear(); // Clear out any old bricks

    // Brick layout parameters
    const int rowCount = 5;
    const int colCount = 10;
    const float brickWidth = 70.0f;
    const float brickHeight = 20.0f;
    const float padding = 5.0f;

    // Calculate the total width of the block of bricks
    float totalBlockWidth = (colCount * brickWidth) + ((colCount - 1) * padding);
    // --- Offset is now relative to the gameArea ---
    float offsetX = gameArea.x + (gameArea.width - totalBlockWidth) / 2.0f;
    float offsetY = gameArea.y + 50.0f; // Start 50px from the top of the game area

    for (int r = 0; r < rowCount; r++)
    {
        for (int c = 0; c < colCount; c++)
        {
            // --- Randomize! ---
            // 50/50 chance for a brick to spawn
            if (GetRandomValue(0, 1) == 1)
            {
                float brickX = offsetX + c * (brickWidth + padding);
                float brickY = offsetY + r * (brickHeight + padding);

                // --- NEW: Set Health, Score, and Color ---
                int brickHealth = GetRandomValue(1, 3);
                int brickScore = brickHealth * 10; // 10, 20, or 30 points
                Color brickColor;

                if (brickHealth == 3)
                    brickColor = GRAY;
                else if (brickHealth == 2)
                    brickColor = ORANGE;
                else
                    brickColor = RED;

                // --- MODIFIED: Use new constructor ---
                bricks.push_back(Brick(brickX, brickY, brickWidth, brickHeight, brickHealth, brickScore, brickColor));
            }
        }
    }
}

int main(void)
{
    // --- 1. DEFINE NEW SCREEN LAYOUT ---
    const int gameAreaWidth = 800;
    const int gameAreaHeight = 900;
    const int uiPanelWidth = 300; // <-- MODIFIED: Was 250, now wider

    const int totalScreenWidth = gameAreaWidth + 2 * uiPanelWidth; // 800 + 300 + 300 = 1400
    const int totalScreenHeight = gameAreaHeight;                  // 900

    Rectangle gameArea = {(float)uiPanelWidth, 0.0f, (float)gameAreaWidth, (float)gameAreaHeight};

    // --- 2. INITIALIZE WITH NEW TOTAL WIDTH ---
    InitWindow(totalScreenWidth, totalScreenHeight, "Brick Breaker Madness");

    std::vector<Ball> balls;
    std::vector<Brick> bricks;

    CreateLevel(bricks, gameArea);

    // --- CREATE THE PADDLE (Relative to gameArea) ---
    float paddleWidth = 100.0f;
    float paddleHeight = 20.0f;
    Paddle paddle(
        gameArea.x + (gameArea.width / 2) - (paddleWidth / 2),
        gameArea.y + gameArea.height - paddleHeight - 20,
        paddleWidth,
        paddleHeight,
        7.0f,
        BLACK);

    bool isChaosMode = false;

    // --- MODIFIED: Counters ---
    int bricksDestroyed = 0;  // Per-level counter for the ratio bar
    int ballsDestroyed = 0;   // Per-level counter for the ratio bar
    int totalScore = 0;       // Per-game high score
    int currentLevel = 1;     // <-- ADDED: Level counter
    bool shouldClose = false; // <-- ADDED: For the quit button

    // --- ADDED: Define Quit Button Rect ---
    float rightPanelX = gameArea.x + gameArea.width;
    float buttonWidth = 120;
    float buttonHeight = 40;
    float buttonX = rightPanelX + (uiPanelWidth - buttonWidth) / 2; // Centered
    float buttonY = totalScreenHeight - buttonHeight - 30;          // 30px from bottom
    Rectangle quitButtonRect = {buttonX, buttonY, buttonWidth, buttonHeight};

    SetTargetFPS(60);

    // --- MODIFIED: Added shouldClose to loop condition ---
    while (!WindowShouldClose() && !shouldClose)
    {
        // --- 1. UPDATE ---

        paddle.Update(gameArea);

        // --- ADDED: Check for Quit Button Click ---
        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, quitButtonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            shouldClose = true;
        }

        if (IsKeyPressed(KEY_C))
        {
            isChaosMode = !isChaosMode; // Flip the boolean
        }

        // --- MODIFIED: Update loop to handle score and destroy count ---
        for (Ball &ball : balls)
        {
            if (ball.isAlive)
            {
                int scoreFromThisBall = ball.Update(paddle, bricks, gameArea);

                if (scoreFromThisBall > 0)
                {
                    totalScore += scoreFromThisBall; // Add to game score
                    bricksDestroyed++;               // A brick was broken!
                }
            }
        }

        // --- 4. "EXPLODE" (Remove) Dead Balls ---
        for (int i = balls.size() - 1; i >= 0; i--)
        {
            if (!balls[i].isAlive)
            {
                ballsDestroyed++;
                balls.erase(balls.begin() + i);
            }
        }

        // --- SPAWN BALL ON CLICK ---
        if (!isChaosMode && balls.empty() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            float newRadius = 10.0f;
            balls.push_back(Ball(
                paddle.x + (paddle.width / 2),
                paddle.y - newRadius,
                newRadius,
                (float)GetRandomValue(-5, 5),
                -7.0f,
                BLACK));
        }
        else if (isChaosMode && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            float newRadius = 10.0f;
            balls.push_back(Ball(
                paddle.x + (paddle.width / 2),
                paddle.y - newRadius,
                newRadius,
                (float)GetRandomValue(-5, 5),
                -7.0f,
                BLACK));
        }

        // --- 6. Check for win condition ---
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
            currentLevel++; // <-- ADDED: Increment the level
            CreateLevel(bricks, gameArea);
            // Reset per-level stats, but NOT totalScore
            bricksDestroyed = 0;
            ballsDestroyed = 0;
        }

        // --- 2. DRAWING ---
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        DrawRectangleRec(gameArea, WHITE);

        // --- 3. DRAW UI (in the new panels) ---

        // --- LEFT PANEL (Stats) ---
        // --- MODIFIED: Reworked all stats text and positions ---
        std::string scoreText = "Score: " + std::to_string(totalScore);
        DrawText(scoreText.c_str(), 10, 10, 30, BLUE);

        std::string levelText = "Level: " + std::to_string(currentLevel);
        DrawText(levelText.c_str(), 10, 50, 30, BLACK);

        std::string modeText = "Mode: ";
        Color modeColor = BLACK;
        if (isChaosMode)
        {
            modeText += "CHAOS!";
            modeColor = MAROON;
        }
        else
        {
            modeText += "Normal";
            modeColor = DARKGREEN;
        }
        DrawText(modeText.c_str(), 10, 90, 25, modeColor); // Y was 50

        // --- MODIFIED: Changed text labels ---
        std::string brickText = "Bricks Broken: " + std::to_string(bricksDestroyed);
        std::string ballText = "Balls Lost: " + std::to_string(ballsDestroyed);
        DrawText(brickText.c_str(), 10, 130, 25, DARKGREEN); // Y was 90
        DrawText(ballText.c_str(), 10, 165, 25, MAROON);     // Y was 125

        // --- Ratio bar (Y-pos updated) ---
        float barWidth = uiPanelWidth - 20.0f;
        float barHeight = 25.0f;
        float barX = 10.0f;
        float barY = 210.0f; // Y was 170
        int totalDestroyed = bricksDestroyed + ballsDestroyed;

        float brickRatio = 0.0f;
        if (totalDestroyed > 0)
        {
            brickRatio = (float)bricksDestroyed / (float)totalDestroyed;
        }

        DrawRectangle(barX, barY, barWidth, barHeight, Fade(MAROON, 0.5f));
        DrawRectangle(barX, barY, (int)(barWidth * brickRatio), barHeight, DARKGREEN);
        DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, BLACK);
        DrawText("Level K/D Ratio", (int)barX, (int)barY + (int)barHeight + 10, 15, BLACK);

        // --- ADDED: FPS Meter ---
        DrawFPS(10, totalScreenHeight - 30); // Draw in bottom-left

        // --- RIGHT PANEL (How to Play) ---
        DrawText("HOW TO PLAY", (int)rightPanelX + 10, 10, 30, BLACK);
        DrawText("Mouse: Move Paddle", (int)rightPanelX + 10, 60, 20, BLACK);
        DrawText("Click: Spawn Ball", (int)rightPanelX + 10, 90, 20, BLACK);
        DrawText("C: Toggle Chaos", (int)rightPanelX + 10, 120, 20, BLACK);

        // --- ADDED: Draw Quit Button ---
        bool isMouseOverButton = CheckCollisionPointRec(mousePos, quitButtonRect);
        Color buttonColor = isMouseOverButton ? RED : MAROON;

        DrawRectangleRec(quitButtonRect, buttonColor);
        DrawText("QUIT GAME", (int)(quitButtonRect.x + (quitButtonRect.width - MeasureText("QUIT GAME", 20)) / 2), (int)(quitButtonRect.y + (quitButtonRect.height - 20) / 2), 20, WHITE);
        DrawRectangleLines((int)quitButtonRect.x, (int)quitButtonRect.y, (int)quitButtonRect.width, (int)quitButtonRect.height, BLACK);

        // --- 4. DRAW GAME OBJECTS (inside gameArea) ---

        for (Brick &brick : bricks)
        {
            brick.Draw();
        }

        paddle.Draw();

        for (Ball &ball : balls)
        {
            ball.Draw();
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}