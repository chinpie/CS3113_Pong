#include <math.h>
#include <string>
#include "CS3113/cs3113.h"

/**

* Author: Tyler Davidovich

* Assignment: Pong Clone

* Date due: [02/24/2026]

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

// Enums

AppStatus gAppStatus = RUNNING;
enum GameMode
{
    SINGLEPLAYER,
    MULTIPLAYER
};

enum BallCount
{
    ONE,
    TWO,
    THREE,
    FOUR
};

enum GameState
{
    PLAYING,
    GAMEOVER
};

GameMode gGameMode = MULTIPLAYER;
BallCount gBallCount = ONE;
GameState gGameState = PLAYING;

// Global Constants

constexpr int SCREEN_HEIGHT = 800,
              SCREEN_WIDTH = 1600,
              FPS = 60,
              SIZE = 100,
              PADDLESPEED = 1200,
              BALLSPEED = 500,
              BALLSIZE = 30;

constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                  BASE_SIZE = {(float)SIZE, (float)SIZE},
                  BASE_BALL_SIZE = {(float)BALLSIZE, (float)BALLSIZE},
                  INIT_POS_P1 = {ORIGIN.x - 600.0f, ORIGIN.y},
                  INIT_POS_P2 = {ORIGIN.x + 600.0f, ORIGIN.y},
                  INIT_BALL_POSITION = {SCREEN_WIDTH / 2, 100},
                  BG_SCALE = {float(SIZE) * 20, float(SIZE) * 8};

constexpr char PADDLEONE_FP[] = "Assets/havel.png",
               PADDLETWO_FP[] = "Assets/Dark_Souls_Sif.png",
               BALL_FP[] = "Assets/ball.png",
               BG_FP[] = "Assets/background.png",
               BG_COLOUR[] = "#000000";

// Global Variables
float gMaxBallAngle = 90.0f,
      gPreviousTicks = 0.0f,
      gAngle = 0.0f,
      gPreviousCollision = -100.0f,
      gCollisionDelay = 0.005f;

Vector2 gPaddleOnePosition = INIT_POS_P1,
        gPaddleOneMovement = {0.0f, 0.0f},
        gBallOnePosition = INIT_BALL_POSITION,
        gBallTwoPosition = {INIT_BALL_POSITION.x - 100, INIT_BALL_POSITION.y + 300},
        gBallThreePosition = {INIT_BALL_POSITION.x - 100, INIT_BALL_POSITION.y + 350},
        gBallFourPosition = {INIT_BALL_POSITION.x - 100, INIT_BALL_POSITION.y + 500},
        gBallOneMovement = {1.0f, 0.0f},
        gBallTwoMovement = {1.0f, 0.0f},
        gBallThreeMovement = {1.0f, 0.0f},
        gBallFourMovement = {1.0f, 0.0f},
        gBallOneScale = BASE_BALL_SIZE,
        gPaddleScale = BASE_SIZE,
        gPaddleTwoPosition = INIT_POS_P2,
        gPaddleTwoMovement = {0.0f, 0.0f},
        gPaddleTwoScale = BASE_SIZE;

int playerOnePoints = 0;
int playerTwoPoints = 0;

std::string playerOneScoreText = "" + std::to_string(playerOnePoints);
std::string playerTwoScoreText = "" + std::to_string(playerTwoPoints);

// Global Textures

Texture2D gPaddleOneTexture;
Texture2D gPaddleTwoTexture;
Texture2D gBallTexture;
Texture2D gBackgroundTexture;

unsigned int startTime;
// Function Declarations

void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f)
        return true;

    return false;
}

void ballCollision(const Vector2 *ballPosition, const Vector2 *ballScale,
                   const Vector2 *paddleOnePosition, const Vector2 *paddleTwoPosition, const Vector2 *paddleScale, Vector2 *ballMovement)
{
    if (isColliding(paddleOnePosition, paddleScale, ballPosition, ballScale))
    {
        ballMovement->x = 1;
        // Check what part of the paddle it is hitting
        if (ballPosition->y <= (paddleScale->y / 2 + paddleOnePosition->y))
        {
            ballMovement->y = -1;
        }
        else if (ballPosition->y > (paddleOnePosition->y - paddleScale->y / 2))
        {
            ballMovement->y = 1;
        }
    }
    else if (isColliding(paddleTwoPosition, paddleScale, ballPosition, ballScale))
    {
        ballMovement->x = -1;
        if (ballPosition->y < (paddleScale->y / 2 + paddleTwoPosition->y))
        {
            ballMovement->y = -1;
        }
        else if (ballPosition->y > (paddleTwoPosition->y - paddleScale->y))
        {
            ballMovement->y = 1;
        }
    }
}

void calculatePoints(const Vector2 *initialPos, Vector2 *ballposition, Vector2 *ballMovement, int &playerOnePoints, int &playerTwoPoints, std::string &playerOneText,
                     std::string &playerTwoText)
{
    if (ballposition->x > 1600.0f)
    {
        playerOnePoints += 1;
        playerOneText = "" + std::to_string(playerOnePoints);
        *ballposition = *initialPos;
        ballMovement->y = 0;
    }
    else if (ballposition->x < 0.0f)
    {
        playerTwoPoints += 1;
        playerTwoText = "" + std::to_string(playerTwoPoints);
        *ballposition = *initialPos;
        ballMovement->y = 0;
    }
}

void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)};

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)};

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f};

    // Render the texture on screen
    DrawTexturePro(
        *texture,
        textureArea, destinationArea, originOffset,
        gAngle, WHITE);
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Clone");
    gPaddleOneTexture = LoadTexture(PADDLEONE_FP);
    gPaddleTwoTexture = LoadTexture(PADDLETWO_FP);
    gBallTexture = LoadTexture(BALL_FP);
    gBackgroundTexture = LoadTexture(BG_FP);
}

void processInput()
{
    gPaddleOneMovement = {0.0f, 0.0f};
    if (gGameMode == MULTIPLAYER)
    {
        gPaddleTwoMovement = {0.0f, 0.0f};
    }

    if (IsKeyPressed(KEY_T))
    {
        gGameMode = SINGLEPLAYER;
        gPaddleTwoMovement.y = 1;
    }
    if (IsKeyPressed(KEY_ONE))
    {
        gBallCount = ONE;
    }
    if (IsKeyPressed(KEY_TWO))
    {
        gBallCount = TWO;
    }
    if (IsKeyPressed(KEY_THREE))
    {
        gBallCount = THREE;
    }
    if (IsKeyPressed(KEY_FOUR))
    {
        gBallCount = FOUR;
    }
    if (IsKeyDown(KEY_W))
        gPaddleOneMovement.y = -1;
    else if (IsKeyDown(KEY_S))
        gPaddleOneMovement.y = 1;
    if (IsKeyDown(KEY_UP) && gGameMode == MULTIPLAYER)
        gPaddleTwoMovement.y = -1;
    else if (IsKeyDown(KEY_DOWN) && gGameMode == MULTIPLAYER)
        gPaddleTwoMovement.y = 1;

    if (IsKeyPressed(KEY_Q) || WindowShouldClose())
        gAppStatus = TERMINATED;
}

void update()
{
    // Delta time calculations
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    if (gGameState == PLAYING)
    {
        if ((gPaddleTwoPosition.y + gPaddleTwoScale.y / 2) >= SCREEN_HEIGHT)
        {
            gPaddleTwoMovement.y = -1;
        }
        if ((gPaddleTwoPosition.y - gPaddleTwoScale.y / 2) <= 0.0f)
        {
            gPaddleTwoMovement.y = 1;
        }
        if ((gPaddleOnePosition.y + gPaddleScale.y / 2) >= SCREEN_HEIGHT)
        {
            gPaddleOneMovement.y = -1;
        }
        if ((gPaddleOnePosition.y - gPaddleScale.y / 2) <= 0.0f)
        {
            gPaddleOneMovement.y = 1;
        }
        gPaddleOnePosition = {
            gPaddleOnePosition.x,
            gPaddleOnePosition.y + PADDLESPEED * gPaddleOneMovement.y * deltaTime};
        gPaddleTwoPosition = {
            gPaddleTwoPosition.x,
            gPaddleTwoPosition.y + PADDLESPEED * gPaddleTwoMovement.y * deltaTime};

        // Collision Detection

        if (gBallOnePosition.y > 800)
        {
            gBallOneMovement.y = -1;
        }
        else if (gBallOnePosition.y < 0)
        {
            gBallOneMovement.y = 1;
        }
        if (gBallTwoPosition.y > 800)
        {
            gBallTwoMovement.y = -1;
        }
        else if (gBallTwoPosition.y < 0)
        {
            gBallTwoMovement.y = 1;
        }
        if (gBallThreePosition.y > 800)
        {
            gBallThreeMovement.y = -1;
        }
        else if (gBallThreePosition.y < 0)
        {
            gBallThreeMovement.y = 1;
        }
        if (gBallFourPosition.y > 800)
        {
            gBallFourMovement.y = -1;
        }
        else if (gBallFourPosition.y < 0)
        {
            gBallFourMovement.y = 1;
        }

        ballCollision(&gBallOnePosition, &gBallOneScale, &gPaddleOnePosition, &gPaddleTwoPosition, &gPaddleScale, &gBallOneMovement);
        ballCollision(&gBallTwoPosition, &gBallOneScale, &gPaddleOnePosition, &gPaddleTwoPosition, &gPaddleScale, &gBallTwoMovement);
        ballCollision(&gBallThreePosition, &gBallOneScale, &gPaddleOnePosition, &gPaddleTwoPosition, &gPaddleScale, &gBallThreeMovement);
        ballCollision(&gBallFourPosition, &gBallOneScale, &gPaddleOnePosition, &gPaddleTwoPosition, &gPaddleScale, &gBallFourMovement);

        gBallOnePosition = {
            gBallOnePosition.x + BALLSPEED * gBallOneMovement.x * deltaTime,
            gBallOnePosition.y + BALLSPEED * gBallOneMovement.y * deltaTime};
        if (gBallCount == TWO || gBallCount == THREE || gBallCount == FOUR)
        {
            gBallTwoPosition = {
                gBallTwoPosition.x + BALLSPEED * gBallTwoMovement.x * deltaTime,
                gBallTwoPosition.y + BALLSPEED * gBallTwoMovement.y * deltaTime};
        }
        if (gBallCount == THREE || gBallCount == FOUR)
        {
            gBallThreePosition = {
                gBallThreePosition.x + BALLSPEED * gBallThreeMovement.x * deltaTime,
                gBallThreePosition.y + BALLSPEED * gBallThreeMovement.y * deltaTime};
        }
        if (gBallCount == FOUR)
        {
            gBallFourPosition = {
                gBallFourPosition.x + BALLSPEED * gBallFourMovement.x * deltaTime,
                gBallFourPosition.y + BALLSPEED * gBallFourMovement.y * deltaTime};
        }

        calculatePoints(&INIT_BALL_POSITION, &gBallOnePosition, &gBallOneMovement, playerOnePoints, playerTwoPoints, playerOneScoreText,
                        playerTwoScoreText);
        calculatePoints(&INIT_BALL_POSITION, &gBallTwoPosition, &gBallTwoMovement, playerOnePoints, playerTwoPoints, playerOneScoreText,
                        playerTwoScoreText);
        calculatePoints(&INIT_BALL_POSITION, &gBallThreePosition, &gBallThreeMovement, playerOnePoints, playerTwoPoints, playerOneScoreText,
                        playerTwoScoreText);
        calculatePoints(&INIT_BALL_POSITION, &gBallFourPosition, &gBallFourMovement, playerOnePoints, playerTwoPoints, playerOneScoreText,
                        playerTwoScoreText);
        if (playerOnePoints == 10 || playerTwoPoints == 10)
        {
            gGameState = GAMEOVER;
        }
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
    if (gGameState == PLAYING)
    {
        renderObject(&gBackgroundTexture, &ORIGIN, &BG_SCALE);
        renderObject(&gPaddleOneTexture, &gPaddleOnePosition, &gPaddleScale);
        renderObject(&gPaddleTwoTexture, &gPaddleTwoPosition, &gPaddleScale);
        renderObject(&gBallTexture, &gBallOnePosition, &gBallOneScale);
        if (gBallCount == TWO || gBallCount == THREE || gBallCount == FOUR)
        {
            renderObject(&gBallTexture, &gBallTwoPosition, &gBallOneScale);
        }
        if (gBallCount == THREE || gBallCount == FOUR)
        {
            renderObject(&gBallTexture, &gBallThreePosition, &gBallOneScale);
        }
        if (gBallCount == FOUR)
        {
            renderObject(&gBallTexture, &gBallFourPosition, &gBallOneScale);
        }

        // Player POINTS

        DrawText(playerOneScoreText.c_str(), SCREEN_WIDTH / 2.0f - 625,
                 50, 100, BLACK);
        DrawText(playerTwoScoreText.c_str(), SCREEN_WIDTH / 2.0f + 550,
                 50, 100, BLACK);

        if (gGameMode == SINGLEPLAYER)
        {
            DrawText("Singleplayer Mode Active", SCREEN_WIDTH / 2.0f - 400,
                     50, 50, RED);
        }
        else if (gGameMode == MULTIPLAYER)
        {
            DrawText("Multiplayer Mode Active", SCREEN_WIDTH / 2.0f - 400,
                     50, 50, RED);
        }
    }
    else if (gGameState == GAMEOVER)
    {
        if (playerOnePoints > playerTwoPoints)
        {
            DrawText("Player One is better at Dark souls!\n\t(P1 Won)", SCREEN_WIDTH / 2.0f - 400,
                     50, 50, RED);
        }
        else if (playerOnePoints == playerTwoPoints)
        {
            DrawText("Aww Man there was a Tie!", SCREEN_WIDTH / 2.0f - 400,
                     50, 50, RED);
        }
        else
        {
            DrawText("Player Two is better at Dark souls!\n\t(P2 Won)", SCREEN_WIDTH / 2.0f - 400,
                     50, 50, RED);
        }
    }

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gPaddleOneTexture);
    UnloadTexture(gPaddleTwoTexture);
    UnloadTexture(gBallTexture);
    CloseWindow();
}
int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }
    shutdown();

    return 0;
}
