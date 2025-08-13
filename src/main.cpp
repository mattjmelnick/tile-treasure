#include "raylib.h"
#include "raymath.h"

const int screenWidth = 1000;
const int screenHeight = 600;
const int squareSize = 50;
const int boardSize = 8 * squareSize;
const int borderWidth = 1;
const int frameThickness = 3;

// position the board on the screen
int startX = (screenWidth - boardSize - 300) / 2;
int startY = (screenHeight - boardSize - 50) / 2;

int main(void)
{
    InitWindow(screenWidth + 200, screenHeight + 200, "Tile Treasure");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // draw the 8x8 board
        for (int row = 0; row < 8; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                Color squareColor = BEIGE;

                int x = startX + (col * squareSize);
                int y = startY + (row * squareSize);

                // add a border around each square
                DrawRectangle(x, y, squareSize, squareSize, squareColor);
                DrawRectangleLinesEx((Rectangle){(float)x, (float)y, (float)squareSize, (float)squareSize}, borderWidth, BLACK);
            }
        }

        // create a frame around the board
        Rectangle frameRect = {
            (float)(startX - frameThickness),
            (float)(startY - frameThickness),
            (float)(boardSize + 2 * frameThickness),
            (float)(boardSize + 2 * frameThickness)};
        DrawRectangleLinesEx(frameRect, frameThickness, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}