#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;
const int BOARD_SIZE = 8;
const int SQUARE_SIZE = 50;
const int BORDER_WIDTH = 1;
const int FRAME_THICKNESS = 3;
const int BOARD_OFFSET = (SCREEN_WIDTH - (BOARD_SIZE * SQUARE_SIZE) - 400) / 2;

// position the board on the screen
int startX = BOARD_OFFSET;
int startY = BOARD_OFFSET;

std::vector<int> randomizeWeights();

int main(void)
{
    InitWindow(SCREEN_WIDTH + 200, SCREEN_HEIGHT + 200, "Tile Treasure");
    SetTargetFPS(60);

    std::vector<int> weightsVector = randomizeWeights();
    std::vector<std::vector<int>> board(BOARD_SIZE, std::vector<int>(BOARD_SIZE));

    int weightsIndex = 0;

    // fill board with random weights in weightsVector
    for (int row = 0; row < BOARD_SIZE; row++)
    {
        for (int col = 0; col < BOARD_SIZE; col++)
        {
            if ((row == 1 && col == 1) ||
                (row == 1 && col == 6) ||
                (row == 6 && col == 1) ||
                (row == 6 && col == 6))
            {
                continue;
            }

            board[row][col] = weightsVector[weightsIndex];
            weightsIndex++;
        }
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // draw the 8x8 board
        for (int row = 0; row < BOARD_SIZE; row++)
        {
            for (int col = 0; col < BOARD_SIZE; col++)
            {
                Color squareColor = BEIGE;

                int posX = startX + (col * SQUARE_SIZE);
                int posY = startY + (row * SQUARE_SIZE);

                // draw the square and add a border
                DrawRectangle(posX, posY, SQUARE_SIZE, SQUARE_SIZE, squareColor);
                DrawRectangleLinesEx((Rectangle){(float)posX, (float)posY, (float)SQUARE_SIZE, (float)SQUARE_SIZE}, BORDER_WIDTH, BLACK);

                // add weight text to the squares
                std::string weightText = std::to_string(board[row][col]);
                int textWidth = MeasureText(weightText.c_str(), 15);
                int textX = posX + (SQUARE_SIZE / 2) - (textWidth / 2);
                int textY = posY + (SQUARE_SIZE / 2) + 8;
                DrawText(weightText.c_str(), textX, textY, 15, BLACK);
            }
        }

        // create a frame around the board
        Rectangle frameRect = {
            (float)(startX - FRAME_THICKNESS),
            (float)(startY - FRAME_THICKNESS),
            (float)((BOARD_SIZE * SQUARE_SIZE) + 2 * FRAME_THICKNESS),
            (float)((BOARD_SIZE * SQUARE_SIZE) + 2 * FRAME_THICKNESS)};
        DrawRectangleLinesEx(frameRect, FRAME_THICKNESS, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

std::vector<int> randomizeWeights()
{
    std::vector<int> weights = {1, 2, 3, 4};
    std::vector<int> weightsVector;

    // fill with copies of weights
    for (int i : weights)
    {
        for (int j = 0; j < 15; j++)
        {
            weightsVector.push_back(i);
        }
    }

    // for (int n : weightsVector)
    // {
    //     std::cout << n << ' ';
    // }
    // std::cout << std::endl;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::shuffle(weightsVector.begin(), weightsVector.end(), generator);

    // for (int n : weightsVector)
    // {
    //     std::cout << n << ' ';
    // }
    // std::cout << std::endl;

    return weightsVector;
}
