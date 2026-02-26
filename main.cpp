#include <math.h>
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

enum BallDirection
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NONE
};

AppStatus gAppStatus = RUNNING;
BallDirection gBallXDirection = RIGHT;
BallDirection gBallYDirection = NONE;

// Global Constants

constexpr int SCREEN_HEIGHT = 800,
              SCREEN_WIDTH = 1600,
              FPS = 60,
              SIZE = 100,
              PADDLESPEED = 1200,
              BALLSPEED = 500,
              BALLSIZE = 50;

constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                  BASE_SIZE = {(float)SIZE, (float)SIZE},
                  BASE_BALL_SIZE = {(float)BALLSIZE, (float)BALLSIZE},
                  INIT_POS_P1 = {ORIGIN.x - 600.0f, ORIGIN.y},
                  INIT_POS_P2 = {ORIGIN.x + 600.0f, ORIGIN.y},
                  INIT_BALL_POSITION = {SCREEN_WIDTH / 2, 100},
                  BG_SCALE = {float(SIZE) * 20, float(SIZE) * 10};

constexpr char PADDLEONE_FP[] = "Assets/havel.png",
               PADDLETWO_FP[] = "Assets/Dark_Souls_Sif.png",
               BALL_FP[] = "Assets/ball.png",
               BG_FP[] = "Assets/background.png",
               BG_COLOUR[] = "#000000";

// Global Variables
float gAngle = 0.0f,
      gPreviousTicks = 0.0f;

Vector2 gPaddleOnePosition = INIT_POS_P1,
        gPaddleOneMovement = {0.0f, 0.0f},
        gBallOnePosition = INIT_BALL_POSITION,
        gBallTwoPosition = INIT_BALL_POSITION,
        gBallThreePosition = INIT_BALL_POSITION,
        gBallOneMovement = {0.0f, 0.0f},
        gBallOneScale = BASE_BALL_SIZE,
        gPaddleScale = BASE_SIZE,
        gPaddleTwoPosition = INIT_POS_P2,
        gPaddleTwoMovement = {0.0f, 0.0f},
        gPaddleTwoScale = BASE_SIZE;

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
    gPaddleTwoMovement = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W))
        gPaddleOneMovement.y = -1;
    else if (IsKeyDown(KEY_S))
        gPaddleOneMovement.y = 1;
    if (IsKeyDown(KEY_UP))
        gPaddleTwoMovement.y = -1;
    else if (IsKeyDown(KEY_DOWN))
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

    gPaddleOnePosition = {
        gPaddleOnePosition.x,
        gPaddleOnePosition.y + PADDLESPEED * gPaddleOneMovement.y * deltaTime};
    gPaddleTwoPosition = {
        gPaddleTwoPosition.x,
        gPaddleTwoPosition.y + PADDLESPEED * gPaddleTwoMovement.y * deltaTime};
    gBallOnePosition = {
        gBallOnePosition.x + BALLSPEED * gBallOneMovement.x * deltaTime,
        gBallOnePosition.y + BALLSPEED * gBallOneMovement.y * deltaTime};
    // Collision Detection
    if (gBallXDirection == RIGHT)
    {
        gBallOneMovement.x = 1;
    }
    else if (gBallXDirection == LEFT)
    {
        gBallOneMovement.x = -1;
    }
    if (gBallYDirection == UP)
    {
        gBallOneMovement.y = -1;
    }
    else if (gBallYDirection == DOWN)
    {
        gBallOneMovement.y = 1;
    }
    else
    {
        gBallOneMovement.y = 0;
    }
    if (isColliding(&gPaddleOnePosition, &gPaddleScale, &gBallOnePosition, &gBallOneScale))
    {
        gBallXDirection = RIGHT;
    }
    else if (isColliding(&gPaddleTwoPosition, &gPaddleScale, &gBallOnePosition, &gBallOneScale))
    {
        gBallXDirection = LEFT;
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
    renderObject(&gBackgroundTexture, &ORIGIN, &BG_SCALE);
    renderObject(&gPaddleOneTexture, &gPaddleOnePosition, &gPaddleScale);
    renderObject(&gPaddleTwoTexture, &gPaddleTwoPosition, &gPaddleScale);
    renderObject(&gBallTexture, &gBallOnePosition, &gBallOneScale);

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gPaddleOneTexture);
    UnloadTexture(gPaddleTwoTexture);
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
