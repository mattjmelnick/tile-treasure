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

// board's starting position
int startX = BOARD_OFFSET;
int startY = BOARD_OFFSET;

struct BoardSquare
{
    int height = SQUARE_SIZE;
    int width = SQUARE_SIZE;
    int row;
    int col;
    int posX;
    int posY;
    int value;
    int weight;
    bool visited = false;
};

std::vector<int> createIntVector(std::vector<int> &vector, int numOfInstances);
void randomizeVector(std::vector<int> &vector);

int main(void)
{
    InitWindow(SCREEN_WIDTH + 200, SCREEN_HEIGHT + 200, "Tile Treasure");
    SetTargetFPS(60);

    std::vector<int> values = {-4, -2, 2, 4, 6, 8};
    std::vector<int> weights = {1, 2, 3, 4};
    std::vector<int> valuesVector = createIntVector(values, 10);
    std::vector<int> weightsVector = createIntVector(weights, 15);

    randomizeVector(valuesVector);
    randomizeVector(weightsVector);

    std::vector<std::vector<BoardSquare>> board(BOARD_SIZE, std::vector<BoardSquare>(BOARD_SIZE));

    int valuesIndex = 0;
    int weightsIndex = 0;

    // fill board with random values and weights
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

            board[row][col].row = row;
            board[row][col].col = col;
            board[row][col].value = valuesVector[valuesIndex];
            board[row][col].weight = weightsVector[weightsIndex];

            valuesIndex++;
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

                board[row][col].posX = posX;
                board[row][col].posY = posY;

                // draw the square and add a border
                DrawRectangle(posX, posY, SQUARE_SIZE, SQUARE_SIZE, squareColor);
                DrawRectangleLinesEx((Rectangle){(float)posX, (float)posY, (float)SQUARE_SIZE, (float)SQUARE_SIZE}, BORDER_WIDTH, BLACK);

                // add value text to the squares
                std::string valueText = std::to_string(board[row][col].value);
                if ((row == 1 && col == 1) ||
                    (row == 1 && col == 6) ||
                    (row == 6 && col == 1) ||
                    (row == 6 && col == 6))
                    valueText = "";
                int valueTextWidth = MeasureText(valueText.c_str(), 25);
                int valueTextX = posX + (SQUARE_SIZE / 2) - (valueTextWidth / 2);
                int valueTextY = posY + (SQUARE_SIZE / 2) - 16;
                DrawText(valueText.c_str(), valueTextX, valueTextY, 25, BLACK);

                // add weight text to the squares
                std::string weightText = std::to_string(board[row][col].weight);
                if ((row == 1 && col == 1) ||
                    (row == 1 && col == 6) ||
                    (row == 6 && col == 1) ||
                    (row == 6 && col == 6))
                    weightText = "";
                int weightTextWidth = MeasureText(weightText.c_str(), 15);
                int weightTextX = posX + (SQUARE_SIZE / 2) - (weightTextWidth / 2);
                int weightTextY = posY + (SQUARE_SIZE / 2) + 8;
                DrawText(weightText.c_str(), weightTextX, weightTextY, 15, BLACK);
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

std::vector<int> createIntVector(std::vector<int> &vector, int numOfInstances)
{
    std::vector<int> intVector;

    for (int i : vector)
    {
        for (int j = 0; j < numOfInstances; j++)
        {
            intVector.push_back(i);
        }
    }

    return intVector;
}

void randomizeVector(std::vector<int> &vector)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::shuffle(vector.begin(), vector.end(), generator);
}
