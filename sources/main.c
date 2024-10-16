#include "math.h"
#include "raylib.h"
#include "utils.h"
Ship ship = {.sprite = {
                 (RL_Vector2){0, -25.0f},
                 (RL_Vector2){-12.5, 12.5},
                 (RL_Vector2){12.5, 12.5},
             }};

void InitGame();
void DrawGame();
void UpdateGame(float dt);
int main() {

  RL_InitWindow(WIN_WIDTH, WIN_HEIGHT, "hello");
  RL_SetTargetFPS(60);

  InitGame();

  while (!RL_WindowShouldClose()) {

    UpdateGame(RL_GetFrameTime());
    RL_BeginDrawing();
    RL_ClearBackground(RL_BLACK);
    DrawGame();
    RL_EndDrawing();

  }
}

void InitGame() { InitShip(&ship, 5); }

void DrawGame() {
  DrawShip(&ship);
  RL_DrawFPS(15, 15);
}

void UpdateGame(float dt) {

  if (ship.bullet.pos.x > WIN_WIDTH || ship.bullet.pos.x < 0)
    ship.bullet.draw_flag = false;
  if (ship.bullet.pos.y > WIN_HEIGHT || ship.bullet.pos.y < 0)
    ship.bullet.draw_flag = false;

  if (RL_IsKeyDown(KEY_SPACE)) {
    if (!ship.bullet.draw_flag) {
      ShootBullet(&ship);
    }
  }

  if (RL_IsKeyDown(KEY_W)) {
    ThrustShip(&ship);
  }

  if (RL_IsKeyDown(KEY_A)) {
    ship.angle -= ROTATION_SPEED * dt;
  }
  if (RL_IsKeyDown(KEY_D)) {
    ship.angle += ROTATION_SPEED * dt;
  }
  ship.pivot.x += ship.vel.x * dt;
  ship.pivot.y += ship.vel.y * dt;
  BoundObjectToScreen(&ship.pivot);

  ship.vel.x += (ship.vel.x / DEACCELARATION) * -1 * dt;
  ship.vel.y += (ship.vel.y / DEACCELARATION) * -1 * dt;

  ship.bullet.pos.x += ship.bullet.vel.x * dt;
  ship.bullet.pos.y += ship.bullet.vel.y * dt;
}
