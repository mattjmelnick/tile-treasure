#include "include/raylib.h"
#include "include/raymath.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int BOARD_SIZE = 8;
const int SQUARE_SIZE = 70;
const int BORDER_WIDTH = 1;
const int FRAME_THICKNESS = 3;
const int BOARD_OFFSET = (SCREEN_WIDTH - (BOARD_SIZE * SQUARE_SIZE) - 500) / 2;
const int MAX_WEIGHT = 24;
const int TABLE_WIDTH = 350;
const int TABLE_HEIGHT = 560;

// board's starting position
int startX = BOARD_OFFSET + 20;
int startY = BOARD_OFFSET + 40;

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
    bool visited;
    Color color;
};

struct GamePiece
{
    int id;
    int row;
    int col;
    float radius;
    Color color;
    int capacity;
    int currentWeight;
    int score;
    bool isComputer;
    bool isCurrentPlayer;
    bool isActive;
    bool isWinner;

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

PlayerTablePositions p1Positions = {190, 25, 175, 65, 175, 90, 175, 115};
PlayerTablePositions p2Positions = {190, 150, 175, 190, 175, 215, 175, 240};
PlayerTablePositions p3Positions = {190, 275, 175, 315, 175, 340, 175, 365};
PlayerTablePositions p4Positions = {190, 400, 175, 440, 175, 465, 175, 490};

GamePiece *selectedPiece = nullptr;
bool dragging = false;
bool isGameOver;
bool isTie;

// function forward declarations
std::vector<int> createIntVector(std::vector<int> &vector, int numOfInstances);
void randomizeVector(std::vector<int> &vector);
void fillBoard(std::vector<std::vector<BoardSquare>> &board, std::vector<int> &valuesVec, std::vector<int> &weightsVec);
void initializeBoard();

void handleMouseInput(GamePiece &piece);
void makeCPUMove(GamePiece &piece);
std::pair<int, int> getBestMoveCoords(std::vector<std::pair<int, int>> legalMoves);
bool movePiece(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board, int newRow, int newCol);
void finishTurn();
int checkRemainingMoves(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board, int row, int col);
bool checkGameOver();
void setWinner();
int countWinner();
void resetGame();

void drawSquareText(int boardSquareInt, int row, int col,
                    int fontSize, int posX, int posY, int yOffset);
void drawBoard(std::vector<std::vector<BoardSquare>> &board);
void drawBoardFrame();

void drawGameTable();
void drawPlayerInformation(std::string player, PlayerTablePositions &playerPositions, GamePiece &piece);
void drawNewGameButton();

void addOutline(Vector2 position, GamePiece &piece);
void drawPiece(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board);
void drawDraggingPiece();

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tile Treasure");
    SetTargetFPS(60);

    initializeBoard();

    bool cpuThinking = false;
    double cpuStartTime = 0.0;
    double cpuDelay = 0.5;

    while (!WindowShouldClose())
    {
        if (!isGameOver)
        {
            GamePiece &current = pieces[piecesIndex];

            if (current.isCurrentPlayer && !current.isComputer)
            {
                handleMouseInput(current);
            }
            else
            {
                if (!cpuThinking)
                {
                    cpuThinking = true;
                    cpuStartTime = GetTime();
                }
                else
                {
                    if (GetTime() - cpuStartTime >= cpuDelay)
                    {
                        makeCPUMove(current);
                        finishTurn();

                        cpuThinking = false;
                    }
                }
            }
        }

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
    int vectorIndex = 0;

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
                board[row][col].color = BEIGE;

                int posX = startX + (col * SQUARE_SIZE);
                int posY = startY + (row * SQUARE_SIZE);

                board[row][col].posX = posX;
                board[row][col].posY = posY;
                continue;
            }

            int posX = startX + (col * SQUARE_SIZE);
            int posY = startY + (row * SQUARE_SIZE);

            board[row][col].posX = posX;
            board[row][col].posY = posY;
            board[row][col].row = row;
            board[row][col].col = col;
            board[row][col].value = valuesVec[vectorIndex];
            board[row][col].weight = weightsVec[vectorIndex];
            board[row][col].visited = false;
            board[row][col].color = BEIGE;

            vectorIndex++;
        }
    }
}

void initializeBoard()
{
    std::vector<int> valuesVector = createIntVector(values, 10);
    std::vector<int> weightsVector = createIntVector(weights, 15);
    randomizeVector(valuesVector);
    randomizeVector(weightsVector);

    fillBoard(board, valuesVector, weightsVector);

    pieces.push_back({1, 1, 1, 25.0f, RED, MAX_WEIGHT, 0, 0, false, true, true, false});    // player 1
    pieces.push_back({2, 1, 6, 25.0f, GREEN, MAX_WEIGHT, 0, 0, true, false, true, false});  // player 2
    pieces.push_back({3, 6, 1, 25.0f, BLUE, MAX_WEIGHT, 0, 0, true, false, true, false});   // player 3
    pieces.push_back({4, 6, 6, 25.0f, YELLOW, MAX_WEIGHT, 0, 0, true, false, true, false}); // player 4
}

void handleMouseInput(GamePiece &piece)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();
        Vector2 piecePos = piece.getPosition(board[piece.row][piece.col]);

        if (piece.isCurrentPlayer && CheckCollisionPointCircle(mouse, piecePos, piece.radius))
        {
            selectedPiece = &piece;
            dragging = true;
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
                        int remainingMoves = checkRemainingMoves(*selectedPiece, board, r, c);

                        if (remainingMoves == 0)
                            selectedPiece->isActive = false;

                        finishTurn();
                    }
                    selectedPiece = nullptr;
                    dragging = false;
                    return;
                }
            }
        }
    }
}

std::pair<int, int> getBestMoveCoords(std::vector<std::pair<int, int>> legalMoves)
{
    int maxValue = -10;
    int minWeight = 5;
    std::pair<int, int> bestMoveCoords;

    for (auto pair : legalMoves)
    {
        int boardValue = board[pair.first][pair.second].value;
        int boardWeight = board[pair.first][pair.second].weight;

        if (boardValue >= maxValue && boardWeight <= minWeight)
        {
            maxValue = boardValue;
            minWeight = boardWeight;
        }
        else if (boardValue > maxValue)
        {
            maxValue = boardValue;
            minWeight = boardWeight;
        }
    }

    for (auto pair : legalMoves)
    {
        if (board[pair.first][pair.second].value == maxValue && board[pair.first][pair.second].weight == minWeight)
            bestMoveCoords = {pair.first, pair.second};
    }

    return bestMoveCoords;
}

void makeCPUMove(GamePiece &piece)
{
    std::vector<std::pair<int, int>> legalMoves;
    for (int i = 0; i < 8; i++)
    {
        int destRow = piece.row + DIRECTION_ROWS_8[i];
        int destCol = piece.col + DIRECTION_COLS_8[i];

        if (destRow >= 0 && destRow < BOARD_SIZE &&
            destCol >= 0 && destCol < BOARD_SIZE &&
            board[destRow][destCol].weight + piece.currentWeight <= piece.capacity &&
            !board[destRow][destCol].visited)
            legalMoves.push_back({destRow, destCol});
    }

    if (legalMoves.empty())
    {
        piece.isActive = false;
        return;
    }

    auto bestVal = getBestMoveCoords(legalMoves);

    // int choice = (legalMoves.size() == 1) ? 0 : GetRandomValue(0, legalMoves.size() - 1);
    // auto [newRow, newCol] = legalMoves[choice];
    int newRow = bestVal.first;
    int newCol = bestVal.second;

    bool isMovable = movePiece(piece, board, newRow, newCol);

    if (isMovable)
    {
        int remainingMoves = checkRemainingMoves(piece, board, newRow, newCol);

        if (remainingMoves == 0)
            piece.isActive = false;
    }
}

bool movePiece(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board, int newRow, int newCol)
{
    BoardSquare &destSquare = board[newRow][newCol];

    if (!piece.isActive || !piece.isCurrentPlayer || destSquare.visited ||
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

void finishTurn()
{
    pieces[piecesIndex].isCurrentPlayer = false;

    isGameOver = checkGameOver();

    if (isGameOver)
    {
        setWinner();
        int winnerCount = countWinner();

        if (winnerCount > 1)
            isTie = true;

        return;
    }

    do
    {
        piecesIndex = (piecesIndex + 1) % pieces.size();

    } while (pieces[piecesIndex].isActive == false);

    pieces[piecesIndex].isCurrentPlayer = true;
}

int checkRemainingMoves(GamePiece &piece, std::vector<std::vector<BoardSquare>> &board, int row, int col)
{
    int remainingMoves = 0;

    for (int i = 0; i < 8; i++)
    {
        int destRow = row + DIRECTION_ROWS_8[i];
        int destCol = col + DIRECTION_COLS_8[i];

        if (destRow >= 0 && destRow < BOARD_SIZE &&
            destCol >= 0 && destCol < BOARD_SIZE &&
            board[destRow][destCol].weight + piece.currentWeight <= piece.capacity &&
            !board[destRow][destCol].visited)
        {
            remainingMoves += 1;
        }
    }

    return remainingMoves;
}

bool checkGameOver()
{
    for (const auto &piece : pieces)
    {
        if (piece.isActive)
            return false;
    }

    return true;
}

void setWinner()
{
    int maxValue = std::max_element(pieces.begin(), pieces.end(),
                                    [](const GamePiece &a, const GamePiece &b)
                                    {
                                        return a.score < b.score;
                                    })
                       ->score;

    int minWeight = std::min_element(pieces.begin(), pieces.end(),
                                     [maxValue](const GamePiece &a, const GamePiece &b)
                                     {
                                         if (a.score != maxValue && b.score != maxValue)
                                             return false;
                                         if (a.score != maxValue)
                                             return false;
                                         if (b.score != maxValue)
                                             return true;
                                         return a.currentWeight < b.currentWeight;
                                     })
                        ->currentWeight;

    for (GamePiece &p : pieces)
    {
        if (p.score == maxValue && p.currentWeight == minWeight)
            p.isWinner = true;
    }
}

int countWinner()
{
    int winnerCount = std::count_if(pieces.begin(), pieces.end(),
                                    [](const GamePiece &p)
                                    {
                                        return p.isWinner;
                                    });

    return winnerCount;
}

void resetGame()
{
    selectedPiece = nullptr;
    dragging = false;
    isGameOver = false;
    isTie = false;
    piecesIndex = 0;
    pieces.clear();
    initializeBoard();
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
            // draw the square and add a border
            DrawRectangle(board[row][col].posX, board[row][col].posY,
                          SQUARE_SIZE, SQUARE_SIZE, board[row][col].color);
            DrawRectangleLinesEx((Rectangle){(float)board[row][col].posX,
                                             (float)board[row][col].posY,
                                             (float)SQUARE_SIZE, (float)SQUARE_SIZE},
                                 BORDER_WIDTH, BLACK);

            // add value text to the squares
            drawSquareText(board[row][col].value, row, col, 30,
                           board[row][col].posX, board[row][col].posY, 20);

            // add weight text to the squares
            drawSquareText(board[row][col].weight, row, col, 20,
                           board[row][col].posX, board[row][col].posY, -10);
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
    DrawRectangle(((SCREEN_WIDTH / 2) + 160), startY, TABLE_WIDTH, TABLE_HEIGHT, BEIGE);
    Rectangle frameRect = {
        (float)(((SCREEN_WIDTH / 2) + 160) - (FRAME_THICKNESS + 1)),
        (float)(startY - (FRAME_THICKNESS + 1)),
        (float)((TABLE_WIDTH) + 2 * (FRAME_THICKNESS + 1)),
        (float)((TABLE_HEIGHT) + 2 * (FRAME_THICKNESS + 1))};
    DrawRectangleLinesEx(frameRect, (FRAME_THICKNESS + 1), BLACK);

    drawPlayerInformation("Player 1 (Red)", p1Positions, pieces[0]);
    drawPlayerInformation("Player 2 (Green)", p2Positions, pieces[1]);
    drawPlayerInformation("Player 3 (Blue)", p3Positions, pieces[2]);
    drawPlayerInformation("Player 4 (Yellow)", p4Positions, pieces[3]);
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

    std::string turnMarker;

    if (piece.isCurrentPlayer)
    {
        turnMarker = "*";
        DrawText(turnMarker.c_str(), playerTextPosition.x + 225, playerTextPosition.y, playerFontSize, BLACK);
    }
    else if (isGameOver && piece.isWinner)
    {
        turnMarker = isTie ? "TIE" : "WINS";
        DrawText(turnMarker.c_str(), playerTextPosition.x + 225, playerTextPosition.y, playerFontSize, BLACK);
        drawNewGameButton();
    }
}

void drawNewGameButton()
{
    Rectangle newGameButton = {280, 700, 180, 50};
    std::string newGameText = "NEW GAME";
    Vector2 newGameTextSize = MeasureTextEx(GetFontDefault(), newGameText.c_str(), 20, 0);
    int newGameTextWidth = MeasureText(newGameText.c_str(), 20);

    int newGameTextX = newGameButton.x + (newGameButton.width / 2) - (newGameTextWidth / 2);
    int newGameTextY = newGameButton.y + (newGameButton.height / 2) - (newGameTextSize.y / 2);
    Color newGameButtonColor = ORANGE;

    if (CheckCollisionPointRec(GetMousePosition(), newGameButton))
    {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            resetGame();
        }
    }

    DrawRectangleRec(newGameButton, newGameButtonColor);
    DrawText(newGameText.c_str(), newGameTextX, newGameTextY, 20, BLACK);
}