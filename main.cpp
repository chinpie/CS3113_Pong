#include <math.h>
#include "CS3113/cs3113.h"

/**

* Author: Tyler Davidovich

* Assignment: Simple 2D Scene

* Date due: [02/14/2026]

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

// enums

enum BoneAttacks
{
    SMALLBONE,
    MIDBONE,
    BIGBONE
};

enum StageCount
{
    STARTING,
    BATTLE,
    GASTERBLASTER
};

// GLOBAL CONSTANTS

constexpr int SCREEN_WIDTH = 1600 / 2,
              SCREEN_HEIGHT = 800,
              FPS = 60,
              SIZE = 70;

constexpr char BG_COLOR[] = "#000000";
constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
constexpr Vector2 BASE_SIZE = {float(SIZE), float(SIZE)};

constexpr char Player_Heart[] = "Assets/heart.png";
constexpr char Player_Border[] = "Assets/heart_border.png";
constexpr char SANS_FP[] = "Assets/sans.png";
constexpr char BONE_FP[] = "Assets/bone.png";
constexpr char BLASTER_FP[] = "Assets/gaster_blaster.png";
constexpr char GASTERATTACK_FP[] = "Assets/gaster_attack.png";
constexpr char CHARATTACK_FP[] = "Assets/attackbar.png";

// Global Variables
AppStatus gAppStatus = RUNNING;
float gScaleFactor = SIZE,
      gAngle = 0.0f,
      gPulseTime = 0.0f,
      gJumpSpeed = 0.0f,
      sansScaleFactor = 3.0f,
      gBoneSpeed = -300.0f,
      gBlasterRotation = 0.0f,
      heartFrequency = 2.0f,
      jumpIncrement = 80.0f;
Vector2 gPosition = ORIGIN;
Vector2 gBonePosition = {750, 405};
Vector2 gScale = BASE_SIZE;
Vector2 gBlasterPosition = {400.0f, 550.0f};
Vector2 gBlasterSize = {100.0f, 100.0f};
int frameCount = 0;

bool hitRightWall = false;
float gBoneScale = 1.0f,
      gHeartSpeed = 100.0f;
Vector2 gBoneBaseSize = {50.0f, 50.0f};
Vector2 gSansScale = {BASE_SIZE.x * sansScaleFactor, BASE_SIZE.y *sansScaleFactor};

// Textures

Texture2D gHeartTexture;
Texture2D gBorderTexture;
Texture2D gSansTexture;
Texture2D gBoneTexture;
Texture2D gBlasterTexture;
Texture2D gBlasterAttackTexture;
Texture2D gCharacterAttack;

// Other Floats
float gPreviousTicks = 0.0f,
      gHeartJump = 454.0f,
      gHeartXPos = 50.0f,
      gHeartJumpUpperLimit = 100.0f,
      gJumpingTime = 0.0f,
      gJumpingPower = 45.0f;

// Setting Enums
StageCount stage = STARTING;
BoneAttacks gcurrentAttack = MIDBONE;

// Function Declarations

void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Undertale Battle");
    InitAudioDevice();
    Sound megalovaniaFx = LoadSound("Assets/megalovania.wav");
    PlaySound(megalovaniaFx);

    gSansTexture = LoadTexture(SANS_FP);
    gHeartTexture = LoadTexture(Player_Heart);
    gBorderTexture = LoadTexture(Player_Border);
    gBoneTexture = LoadTexture(BONE_FP);
    gBlasterTexture = LoadTexture(BLASTER_FP);
    gBlasterAttackTexture = LoadTexture(GASTERATTACK_FP);
    gCharacterAttack = LoadTexture(CHARATTACK_FP);
    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose())
        gAppStatus = TERMINATED;
}

void update()
{
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gPulseTime += 1.0f * deltaTime;
    frameCount += 1;
    if (stage == STARTING)
    {
        if (gcurrentAttack == MIDBONE)
        {
            gBonePosition.y = 500.0f;
            gBoneScale = 5.0f;
        }
        if (gBonePosition.x > SCREEN_WIDTH / 2 + 25)
        {
            gBonePosition.x -= 150.0f * deltaTime;
        }
        if (gBonePosition.x <= SCREEN_WIDTH / 2 + 25)
        {
            stage = BATTLE;
            gBonePosition.x = 790.0f;
        }
    }
    if (stage == BATTLE)
    {
        if (frameCount % 600 == 0)
        {
            stage = GASTERBLASTER;
        }
        gJumpingTime += 1.0f * deltaTime;
        gHeartJump = -1 * abs(gJumpingPower * sin(heartFrequency * gPulseTime)) + 770.0f;

        if (gHeartXPos > 750.0f)
        {
            hitRightWall = true;
        }
        if (gHeartXPos < 50.0f)
        {
            hitRightWall = false;
        }

        if (-1 * gJumpingPower + 770.0f > 450.0f)
        {
            gJumpingPower += 500.0f * deltaTime;
        }
        else
        {
            gJumpingPower += 0.0f * deltaTime;
        }
        if (abs(gHeartXPos - (gBonePosition.x + 10)) <= 550 && gHeartJump < gBonePosition.y)
        {
            if (hitRightWall)
            {
                gHeartSpeed = -270.0f;
            }
            if (!hitRightWall)
            {
                gHeartSpeed = 270.0f;
            }
            gJumpingPower += 0.0f * deltaTime;
        }
        else if (hitRightWall)
        {
            gHeartSpeed = -100.0f;
        }
        else
        {
            gHeartSpeed = 100.0f;
        }

        // Bone Direction Changes
        if (hitRightWall && gBonePosition.x < 40.0f)
        {
            gBoneSpeed = 300.0f;
        }
        else if (!hitRightWall && gBonePosition.x > 800.0f)
        {
            gBoneSpeed = -300.0f;
        }

        gHeartXPos += gHeartSpeed * deltaTime;
        gBonePosition.x += gBoneSpeed * deltaTime;

        if (gBonePosition.x < 0.0f || gBonePosition.x > 800.0f)
        {
            if (gcurrentAttack == SMALLBONE)
            {
                gcurrentAttack = MIDBONE;
            }
            else if (gcurrentAttack == MIDBONE)
            {

                gcurrentAttack = BIGBONE;
            }
            else
            {
                gcurrentAttack = SMALLBONE;
            }
            if (gBonePosition.x > 800.0)
            {
                gBonePosition.x = 0.0f;
            }
            else if (gBonePosition.x < 0.0f)
            {
                gBonePosition.x = 750.0f;
            }
        }

        if (gcurrentAttack == SMALLBONE)
        {
            gBoneScale = 1.0f;
            gBonePosition.y = 755.0f;
        }
        else if (gcurrentAttack == MIDBONE)
        {
            gBonePosition.y = 555.0f;
            gBoneScale = 5.0f;
        }
        else
        {
            gBonePosition.y = 560.0f;
            gBoneScale = 5.0f;
        }
    }

    if (stage == GASTERBLASTER)
    {
        if (frameCount % 400 == 0)
        {
            stage = BATTLE;
        }
        gHeartJump = 120 * sin((heartFrequency - .5) * gPulseTime) + 600;
        gHeartXPos = 120 * cos((heartFrequency - .5) * gPulseTime) + 400;
    }

    // AT THE BONE"S X POSITION THE HEART MUST BE AT THE SAME X POSITION WHILE BEING AT THE AMPLITUDE OF ITS

    // EVERY X FRAMES SWITCH TO THE GASTER BLASTER ROTATING WITH THE BEAM
    gBlasterRotation += 100.0f * deltaTime;

    gSansScale = {BASE_SIZE.x * sansScaleFactor + 6.0f * cos(10 * gPulseTime),
                  BASE_SIZE.y * sansScaleFactor + 6.0f * cos(10 * gPulseTime)};
}

void render()
{
    BeginDrawing();

    ClearBackground(BLACK);
    if (stage == GASTERBLASTER)
    {
        ClearBackground(GRAY);
    }

    // Whole texture (UV coordinates)
    Rectangle heartTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gHeartTexture.width),
        static_cast<float>(gHeartTexture.height)};

    Rectangle heartDestinationArea = {
        gHeartXPos,
        gHeartJump,
        gScale.x / 2,
        gScale.y / 2};

    Rectangle borderTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gBorderTexture.width),
        static_cast<float>(gBorderTexture.height)};

    Rectangle borderDestinationArea = {
        50.0f,
        400.0f,
        gScale.x * 11,
        gScale.y * 6};

    Rectangle charAttackTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gCharacterAttack.width),
        static_cast<float>(gCharacterAttack.height)};

    Rectangle charAttackDestinationArea = {
        100.0f,
        500.0f,
        gScale.x * 10,
        gScale.y * 5};

    Rectangle sansTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gSansTexture.width),
        static_cast<float>(gSansTexture.height)};

    Rectangle sansDestinationArea = {
        gPosition.x - 65,
        gPosition.y - 300,
        gSansScale.x,
        gSansScale.y};
    Rectangle boneTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gBoneTexture.width),
        static_cast<float>(gBoneTexture.height)};

    Rectangle boneDestinationArea = {
        gBonePosition.x,
        gBonePosition.y,
        gBoneBaseSize.x,
        gBoneBaseSize.y * gBoneScale};

    Rectangle blasterTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gBlasterTexture.width),
        static_cast<float>(gBlasterTexture.height)};

    Rectangle blasterDestinationArea = {
        gBlasterPosition.x,
        gBlasterPosition.y,
        gBlasterSize.x,
        gBlasterSize.y};
    Rectangle blasterAttackTextureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gBlasterAttackTexture.width),
        static_cast<float>(gBlasterAttackTexture.height)};

    Rectangle blasterAttackDestinationArea = {
        gBlasterPosition.x,
        gBlasterPosition.y + 20,
        gBlasterSize.x,
        gBlasterSize.y * 4};

    Vector2 originOffset = {
        gScale.x / 2.0f, gScale.y / 2.0f};
    Vector2 blasterOrigin = {
        gBlasterSize.x / 2, gBlasterSize.x / 2};
    DrawTexturePro(gSansTexture, sansTextureArea, sansDestinationArea, originOffset, 0.0f, WHITE);
    if (stage == STARTING)
    {
        DrawTexturePro(gCharacterAttack, charAttackTextureArea, charAttackDestinationArea, originOffset, 0.0f, WHITE);
        DrawTexturePro(gBoneTexture, boneTextureArea, boneDestinationArea, originOffset, 0.0f, WHITE);
    }
    if (stage == BATTLE)
    {
        DrawTexturePro(gBoneTexture, boneTextureArea, boneDestinationArea, originOffset, 0.0f, WHITE);
        DrawTexturePro(gHeartTexture, heartTextureArea, heartDestinationArea, originOffset, 0.0f, WHITE);
    }
    if (stage == GASTERBLASTER)
    {
        DrawTexturePro(gHeartTexture, heartTextureArea, heartDestinationArea, originOffset, 0.0f, WHITE);
        DrawTexturePro(gBlasterTexture, blasterTextureArea, blasterDestinationArea, blasterOrigin, gBlasterRotation, WHITE);
        DrawTexturePro(gBlasterAttackTexture, blasterAttackTextureArea, blasterAttackDestinationArea, blasterOrigin, gBlasterRotation, WHITE);
    }
    DrawTexturePro(gBorderTexture, borderTextureArea, borderDestinationArea, originOffset, 0.0f, WHITE);

    EndDrawing();
}

void shutdown()
{
    CloseAudioDevice();
    CloseWindow();
    UnloadTexture(gHeartTexture);
    UnloadTexture(gBoneTexture);
    UnloadTexture(gBorderTexture);
    UnloadTexture(gSansTexture);
    UnloadTexture(gBlasterTexture);
    UnloadTexture(gBlasterAttackTexture);
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