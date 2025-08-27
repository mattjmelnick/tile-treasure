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
const int SQUARE_SIZE = 70;
const int BORDER_WIDTH = 1;
const int FRAME_THICKNESS = 3;
const int BOARD_OFFSET = (SCREEN_WIDTH - (BOARD_SIZE * SQUARE_SIZE) - 300) / 2;
const int MAX_WEIGHT = 24;
const int TABLE_WIDTH = 350;
const int TABLE_HEIGHT = 560;

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
    Color color = BEIGE;
};

struct GamePiece
{
    int row;
    int col;
    float radius;
    Color color;
    int capacity;
    int currentWeight;
    int score;
    bool currentTurn;
    bool isActive;

    Vector2 getPosition(const BoardSquare &square) const
    {
        return {square.posX + square.width / 2.0f,
                square.posY + square.height / 2.0f};
    }
};

struct PlayerTablePositions
{
    int playerLabelOffsetX;
    int playerLabelOffsetY;
    int playerScoreOffsetX;
    int playerScoreOffsetY;
    int playerWeightOffsetX;
    int playerWeightOffsetY;
    int playerCurrentSquareOffsetX;
    int playerCurrentSquareOffsetY;
};

std::vector<int> values = {-4, -2, 2, 4, 6, 8};
std::vector<int> weights = {1, 2, 3, 4};

std::vector<std::vector<BoardSquare>> board(BOARD_SIZE, std::vector<BoardSquare>(BOARD_SIZE));
std::vector<GamePiece> pieces;
int piecesIndex = 0;

const std::array<int, 8> DIRECTION_ROWS_8 = {-1, -1, -1, 0, 0, 1, 1, 1};
const std::array<int, 8> DIRECTION_COLS_8 = {-1, 0, 1, -1, 1, -1, 0, 1};

PlayerTablePositions p1Positions = {280, 25, 265, 65, 265, 90, 265, 115};
PlayerTablePositions p2Positions = {280, 150, 265, 190, 265, 215, 265, 240};

GamePiece *selectedPiece = nullptr;
bool dragging = false;

// function forward declarations
std::vector<int> createIntVector(std::vector<int> &vector, int numOfInstances);
void randomizeVector(std::vector<int> &vector);

void fillBoard(std::vector<std::vector<BoardSquare>> &board, std::vector<int> &valuesVec, std::vector<int> &weightsVec);
void drawSquareText(int boardSquareInt, int row, int col,
                    int fontSize, int posX, int posY, int yOffset);
void drawBoard(std::vector<std::vector<BoardSquare>> &board);
void drawBoardFrame();

void drawGameTable();
void drawPlayerInformation(std::string player, PlayerTablePositions &playerPositions, GamePiece &piece);

bool movePiece(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board, int newRow, int newCol);
void addOutline(Vector2 position, GamePiece &piece);
void drawPiece(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board);
void drawDraggingPiece();

int checkRemainingMoves(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board,
                        const std::array<int, 8> &DIRECTION_ROWS_8, const std::array<int, 8> &DIRECTION_COLS_8,
                        int row, int col);

void handleMouseInput();

int main(void)
{
    InitWindow(SCREEN_WIDTH + 200, SCREEN_HEIGHT + 200, "Tile Treasure");
    SetTargetFPS(60);

    std::vector<int> valuesVector = createIntVector(values, 10);
    std::vector<int> weightsVector = createIntVector(weights, 15);
    randomizeVector(valuesVector);
    randomizeVector(weightsVector);

    fillBoard(board, valuesVector, weightsVector);

    pieces.push_back({1, 1, 25.0f, RED, MAX_WEIGHT, 0, 0, true, true});   // player 1
    pieces.push_back({1, 6, 25.0f, BLUE, MAX_WEIGHT, 0, 0, false, true}); // player 2

    while (!WindowShouldClose())
    {
        handleMouseInput();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        drawBoard(board);
        drawBoardFrame();
        drawGameTable();

        drawDraggingPiece();

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
                board[row][col].visited = true;
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
            int posX = startX + (col * SQUARE_SIZE);
            int posY = startY + (row * SQUARE_SIZE);

            board[row][col].posX = posX;
            board[row][col].posY = posY;

            // draw the square and add a border
            DrawRectangle(posX, posY, SQUARE_SIZE, SQUARE_SIZE, board[row][col].color);
            DrawRectangleLinesEx((Rectangle){(float)posX, (float)posY, (float)SQUARE_SIZE, (float)SQUARE_SIZE}, BORDER_WIDTH, BLACK);

            // add value text to the squares
            drawSquareText(board[row][col].value, row, col, 30, posX, posY, 20);

            // add weight text to the squares
            drawSquareText(board[row][col].weight, row, col, 20, posX, posY, -10);
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

void drawGameTable()
{
    // main table window
    DrawRectangle(((SCREEN_WIDTH / 2) + 250), startY, TABLE_WIDTH, TABLE_HEIGHT, BEIGE);
    Rectangle frameRect = {
        (float)(((SCREEN_WIDTH / 2) + 250) - (FRAME_THICKNESS + 1)),
        (float)(startY - (FRAME_THICKNESS + 1)),
        (float)((TABLE_WIDTH) + 2 * (FRAME_THICKNESS + 1)),
        (float)((TABLE_HEIGHT) + 2 * (FRAME_THICKNESS + 1))};
    DrawRectangleLinesEx(frameRect, (FRAME_THICKNESS + 1), BLACK);

    drawPlayerInformation("Player 1", p1Positions, pieces[0]);
    drawPlayerInformation("Player 2", p2Positions, pieces[1]);

    // TODO: create drawCurrentTurn box to show current player number/color
}

void drawPlayerInformation(std::string player, PlayerTablePositions &playerPositions, GamePiece &piece)
{
    float underlineThickness = 2.0f;
    float underlineOffset = 3.0f;
    int playerFontSize = 25;
    int playerValueFontSize = 20;

    std::string playerText = player;
    Vector2 playerTextPosition = {(float)((SCREEN_WIDTH / 2) + playerPositions.playerLabelOffsetX),
                                  (float)(startY + playerPositions.playerLabelOffsetY)};
    Vector2 playerTextUnderlineStart = {playerTextPosition.x, playerTextPosition.y + playerFontSize + underlineOffset};
    Vector2 playerTextUnderlineEnd = {playerTextPosition.x + MeasureText(playerText.c_str(), playerFontSize),
                                      playerTextPosition.y + playerFontSize + underlineOffset};
    DrawText(playerText.c_str(), playerTextPosition.x, playerTextPosition.y, playerFontSize, BLACK);
    DrawLineEx(playerTextUnderlineStart, playerTextUnderlineEnd, underlineThickness, BLACK);

    Vector2 playerScoreTextPosition = {(float)((SCREEN_WIDTH / 2) + playerPositions.playerScoreOffsetX),
                                       (float)(startY + playerPositions.playerScoreOffsetY)};
    DrawText(TextFormat("Score: %d", piece.score), playerScoreTextPosition.x,
             playerScoreTextPosition.y, playerValueFontSize, BLACK);

    Vector2 playerWeightTextPosition = {(float)((SCREEN_WIDTH / 2) + playerPositions.playerWeightOffsetX),
                                        (float)(startY + playerPositions.playerWeightOffsetY)};
    DrawText(TextFormat("Weight: %d/24", piece.currentWeight), playerWeightTextPosition.x,
             playerWeightTextPosition.y, playerValueFontSize, BLACK);

    Vector2 playerCurrentSquareText = {(float)((SCREEN_WIDTH / 2) + playerPositions.playerCurrentSquareOffsetX),
                                       (float)(startY + playerPositions.playerCurrentSquareOffsetY)};
    DrawText(TextFormat("Current Square: %d/%d", board[piece.row][piece.col].value, board[piece.row][piece.col].weight),
             playerCurrentSquareText.x, playerCurrentSquareText.y, playerValueFontSize, BLACK);
}

bool movePiece(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board, int newRow, int newCol)
{
    BoardSquare &destSquare = board[newRow][newCol];

    if (!piece.isActive || !piece.currentTurn || destSquare.visited ||
        std::max(abs(newRow - piece.row), abs(newCol - piece.col)) != 1)
        return false;

    if (!destSquare.visited && (piece.currentWeight + destSquare.weight <= piece.capacity))
    {
        piece.currentWeight += destSquare.weight;
        piece.score += destSquare.value;
        destSquare.visited = true;
        destSquare.color = piece.color;
        piece.row = newRow;
        piece.col = newCol;
        return true;
    }

    return false;
}

void addOutline(Vector2 position, GamePiece &piece)
{
    DrawCircleV(position, piece.radius, piece.color);
    DrawCircleLinesV(position, piece.radius, BLACK);
    DrawCircleLinesV(position, piece.radius + 0.5, BLACK);
    DrawCircleLinesV(position, piece.radius + 1, BLACK);
}

void drawPiece(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board)
{
    BoardSquare &sq = board[piece.row][piece.col];
    Vector2 pos = {
        sq.posX + sq.width / 2.0f,
        sq.posY + sq.height / 2.0f};
    addOutline(pos, piece);
}

void drawDraggingPiece()
{
    for (auto &piece : pieces)
    {
        if (!dragging || selectedPiece != &piece)
        {
            drawPiece(piece, board);
        }
    }

    if (dragging && selectedPiece)
    {
        Vector2 mouse = GetMousePosition();
        addOutline(mouse, *selectedPiece);
    }
}

int checkRemainingMoves(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board,
                        const std::array<int, 8> &DIRECTION_ROWS_8, const std::array<int, 8> &DIRECTION_COLS_8,
                        int row, int col)
{
    int remainingMoves = 0;
    int legalMove = 0;

    for (int i = 0; i < 8; i++)
    {
        int destRow = row + DIRECTION_ROWS_8[i];
        int destCol = col + DIRECTION_COLS_8[i];

        if (destRow >= 0 && destRow < BOARD_SIZE &&
            destCol >= 0 && destCol < BOARD_SIZE)
        {
            legalMove = board[destRow][destCol].visited ? 0 : 1;
            remainingMoves += legalMove;
        }
    }

    return remainingMoves;
}

void handleMouseInput()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();

        for (auto &piece : pieces)
        {
            Vector2 piecePos = piece.getPosition(board[piece.row][piece.col]);

            if (CheckCollisionPointCircle(mouse, piecePos, piece.radius))
            {
                selectedPiece = &piece;
                dragging = true;
                break;
            }
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && dragging && selectedPiece)
    {
        Vector2 mouse = GetMousePosition();

        for (int r = 0; r < BOARD_SIZE; r++)
        {
            for (int c = 0; c < BOARD_SIZE; c++)
            {
                BoardSquare &sq = board[r][c];

                Rectangle rect = {(float)sq.posX,
                                  (float)sq.posY,
                                  (float)sq.width,
                                  (float)sq.height};

                if (CheckCollisionPointRec(mouse, rect))
                {
                    bool isMovable = movePiece(*selectedPiece, board, r, c);

                    if (isMovable)
                    {
                        // TODO: create updateGameState function to check for changes after each move
                        int remainingMoves = checkRemainingMoves(*selectedPiece, board, DIRECTION_ROWS_8, DIRECTION_COLS_8, r, c);
                        if (remainingMoves == 0)
                            selectedPiece->isActive = false;
                        std::cout << remainingMoves << "\n";

                        pieces[piecesIndex].currentTurn = false;
                        piecesIndex += 1;

                        if (piecesIndex >= pieces.size())
                            piecesIndex = 0;

                        pieces[piecesIndex].currentTurn = true;
                    }
                    break;
                }
            }
        }

        selectedPiece = nullptr;
        dragging = false;
    }
}