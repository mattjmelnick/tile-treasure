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

std::vector<int> values = {-4, -2, 2, 4, 6, 8};
std::vector<int> weights = {1, 2, 3, 4};

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

// function forward declarations
std::vector<int> createIntVector(std::vector<int> &vector, int numOfInstances);
void randomizeVector(std::vector<int> &vector);
void fillBoard(std::vector<std::vector<BoardSquare>> &board, std::vector<int> &valuesVec, std::vector<int> &weightsVec);
void drawSquareText(int boardSquareInt, int row, int col,
                    int fontSize, int posX, int posY, int yOffset);
void drawBoard(std::vector<std::vector<BoardSquare>> &board);
void drawBoardFrame();

int main(void)
{
    InitWindow(SCREEN_WIDTH + 200, SCREEN_HEIGHT + 200, "Tile Treasure");
    SetTargetFPS(60);

    std::vector<int> valuesVector = createIntVector(values, 10);
    std::vector<int> weightsVector = createIntVector(weights, 15);

    randomizeVector(valuesVector);
    randomizeVector(weightsVector);

    std::vector<std::vector<BoardSquare>> board(BOARD_SIZE, std::vector<BoardSquare>(BOARD_SIZE));
    fillBoard(board, valuesVector, weightsVector);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        drawBoard(board);
        drawBoardFrame();

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

void fillBoard(std::vector<std::vector<BoardSquare>> &board,
               std::vector<int> &valuesVec,
               std::vector<int> &weightsVec)
{
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
            board[row][col].value = valuesVec[valuesIndex];
            board[row][col].weight = weightsVec[weightsIndex];

            valuesIndex++;
            weightsIndex++;
        }
    }
}

void drawSquareText(int boardSquareInt, int row, int col,
                    int fontSize, int posX, int posY, int yOffset)
{
    std::string valueText = std::to_string(boardSquareInt);
    if ((row == 1 && col == 1) ||
        (row == 1 && col == 6) ||
        (row == 6 && col == 1) ||
        (row == 6 && col == 6))
        valueText = "";
    int valueTextWidth = MeasureText(valueText.c_str(), fontSize);
    int valueTextX = posX + (SQUARE_SIZE / 2) - (valueTextWidth / 2);
    int valueTextY = posY + (SQUARE_SIZE / 2) - yOffset;
    DrawText(valueText.c_str(), valueTextX, valueTextY, fontSize, BLACK);
}

void drawBoard(std::vector<std::vector<BoardSquare>> &board)
{
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
            drawSquareText(board[row][col].value, row, col, 25, posX, posY, 16);

            // add weight text to the squares
            drawSquareText(board[row][col].weight, row, col, 15, posX, posY, -8);
        }
    }
}

void drawBoardFrame()
{
    // create a frame around the board
    Rectangle frameRect = {
        (float)(startX - FRAME_THICKNESS),
        (float)(startY - FRAME_THICKNESS),
        (float)((BOARD_SIZE * SQUARE_SIZE) + 2 * FRAME_THICKNESS),
        (float)((BOARD_SIZE * SQUARE_SIZE) + 2 * FRAME_THICKNESS)};
    DrawRectangleLinesEx(frameRect, FRAME_THICKNESS, BLACK);
}