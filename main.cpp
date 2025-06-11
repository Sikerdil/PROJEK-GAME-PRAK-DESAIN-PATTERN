#include "raylib.h"
#include "raymath.h"
#include <cstdlib>

#define MAX_BULLETS 50
#define MAX_ENEMIES 50

enum GameScreen { MENU, GAMEPLAY, GAMEOVER };

struct Bullet {
    Vector2 position;
    Vector2 direction;
    bool active = false;
};

struct Enemy {
    Vector2 position;
    bool active = false;
};

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Top-down Shooter with raylib");
    SetTargetFPS(60);

    GameScreen currentScreen = MENU;

    Vector2 playerPos = { screenWidth / 2.0f, screenHeight / 2.0f };
    float playerSpeed = 5.0f;

    Bullet bullets[MAX_BULLETS] = {};
    Enemy enemies[MAX_ENEMIES] = {};
    int score = 0;
    float enemySpawnTimer = 0.0f;
    float enemySpawnRate = 1.0f;

    Texture2D playerTexture = LoadTexture("player.png");
    Texture2D enemyTexture = LoadTexture("enemy.png");
    Texture2D menuTexture = LoadTexture("layar.png");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentScreen == MENU) {
            DrawTexture(menuTexture, 0, 0, WHITE);
            DrawText("TOP-DOWN SHOOTER", 220, 180, 40, DARKGRAY);
            DrawText("Press ENTER to Start", 280, 260, 20, DARKGRAY);

            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = GAMEPLAY;
                score = 0;
                playerPos = { screenWidth / 2.0f, screenHeight / 2.0f };
                for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
                for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
            }
        }

        else if (currentScreen == GAMEPLAY) {
            if (IsKeyDown(KEY_W)) playerPos.y -= playerSpeed;
            if (IsKeyDown(KEY_S)) playerPos.y += playerSpeed;
            if (IsKeyDown(KEY_A)) playerPos.x -= playerSpeed;
            if (IsKeyDown(KEY_D)) playerPos.x += playerSpeed;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].position = playerPos;
                        bullets[i].direction = Vector2Normalize(Vector2Subtract(GetMousePosition(), playerPos));
                        bullets[i].active = true;
                        break;
                    }
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) {
                    bullets[i].position = Vector2Add(bullets[i].position, Vector2Scale(bullets[i].direction, 10.0f));
                    if (bullets[i].position.x < 0 || bullets[i].position.x > screenWidth ||
                        bullets[i].position.y < 0 || bullets[i].position.y > screenHeight) {
                        bullets[i].active = false;
                    }
                }
            }

            enemySpawnTimer += GetFrameTime();
            if (enemySpawnTimer >= enemySpawnRate) {
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (!enemies[i].active) {
                        Vector2 spawnPos;
                        float minDistance = 100.0f;
                        do {
                            spawnPos = { (float)(rand() % screenWidth), (float)(rand() % screenHeight) };
                        } while (Vector2Distance(spawnPos, playerPos) < minDistance);

                        enemies[i].position = spawnPos;
                        enemies[i].active = true;
                        break;
                    }
                }
                enemySpawnTimer = 0.0f;
            }

            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) {
                    Vector2 toPlayer = Vector2Subtract(playerPos, enemies[i].position);
                    enemies[i].position = Vector2Add(enemies[i].position, Vector2Scale(Vector2Normalize(toPlayer), 1.5f));

                    float enemyRadius = 15.0f;
                    float playerRadius = 24.0f;

                    if (CheckCollisionCircles(enemies[i].position, enemyRadius, playerPos, playerRadius)) {
                        currentScreen = GAMEOVER;
                    }
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bullets[i].active) continue;
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (enemies[j].active) {
                        float enemyRadius = 20.0f;
                        if (CheckCollisionCircles(bullets[i].position, 5, enemies[j].position, enemyRadius)) {
                            bullets[i].active = false;
                            enemies[j].active = false;
                            score++;
                            break;
                        }
                    }
                }
            }

            // Gambar player dengan ukuran 10% dari ukuran aslinya
            float playerScale = 0.1f;
            Rectangle sourceRec = { 0, 0, (float)playerTexture.width, (float)playerTexture.height };
            Rectangle destRec = { playerPos.x, playerPos.y,
                                  playerTexture.width * playerScale,
                                  playerTexture.height * playerScale };
            Vector2 origin = { (playerTexture.width * playerScale) / 2.0f, (playerTexture.height * playerScale) / 2.0f };

            // Collider Player (DEBUG) - transparan dan digambar dulu
            float playerRadius = 24.0f;
            DrawCircleLines((int)playerPos.x, (int)playerPos.y, playerRadius, Fade(RED, 0.2f));

            DrawTexturePro(playerTexture, sourceRec, destRec, origin, 0.0f, WHITE);

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active)
                    DrawCircleV(bullets[i].position, 5, RED);
            }

            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) {
                    Rectangle src = { 0, 0, (float)enemyTexture.width, (float)enemyTexture.height };
                    Rectangle dst = { enemies[i].position.x, enemies[i].position.y,
                                      (float)enemyTexture.width, (float)enemyTexture.height };
                    Vector2 org = { enemyTexture.width / 2.0f, enemyTexture.height / 2.0f };
                    DrawTexturePro(enemyTexture, src, dst, org, 0.0f, WHITE);
                }
            }

            DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);
        }

        else if (currentScreen == GAMEOVER) {
            DrawText("GAME OVER", 300, 200, 40, RED);
            DrawText(TextFormat("Final Score: %d", score), 300, 260, 20, DARKGRAY);
            DrawText("Press ENTER to return to Menu", 240, 320, 20, DARKGRAY);
            if (IsKeyPressed(KEY_ENTER)) currentScreen = MENU;
        }

        EndDrawing();
    }

    UnloadTexture(playerTexture);
    UnloadTexture(enemyTexture);
    UnloadTexture(menuTexture);
    CloseWindow();
    return 0;
}