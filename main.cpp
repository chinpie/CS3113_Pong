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

AppStatus gAppStatus = RUNNING;

// Global Constants

constexpr int SCREEN_HEIGHT = 800,
              SCREEN_WIDTH = 1600,
              FPS = 60,
              SIZE = 500 / 2,
              PADDLESPEED = 200;

constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                  BASE_SIZE = {(float)SIZE, (float)SIZE},
                  INIT_POS_P1 = {ORIGIN.x - 200.0f, ORIGIN.y},
                  INIT_POS_P2 = {ORIGIN.x + 200.0f, ORIGIN.y};

constexpr char PADDLEONE_FP[] = "Assets/havel.png",
               PADDLETWO_FP[] = "Assets/Dark_Souls_Sif.png",
               BG_COLOUR[] = "#000000";

// Global Variables
float gAngle = 0.0f,
      gPreviousTicks = 0.0f;

Vector2 gPaddleOnePosition = INIT_POS_P1,
        gPaddleOneMovement = {0.0f, 0.0f},
        gPaddleScale = BASE_SIZE,
        gPaddleTwoPosition = INIT_POS_P2,
        gPaddleTwoMovement = {0.0f, 0.0f},
        gPaddleTwoScale = BASE_SIZE;

// Global Textures

Texture2D gPaddleOneTexture;
Texture2D gPaddleTwoTexture;

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
}

void processInput()
{
    if (WindowShouldClose())
        gAppStatus = TERMINATED;
}

void update()
{
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));
    renderObject(&gPaddleOneTexture, &gPaddleOnePosition, &gPaddleScale);
    renderObject(&gPaddleTwoTexture, &gPaddleTwoPosition, &gPaddleScale);

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
