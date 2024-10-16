#include "utils.h"
#include "raymath.h"
void SetPivot(Ship *ship) {

  int sprite_size = LEN(ship->sprite);
  ship->pivot = (RL_Vector2){0, 0};
  for (int i = 0; i < sprite_size; i++) {
    ship->pivot.x += ship->model[i].x;
    ship->pivot.y += ship->model[i].y;
  }
  ship->pivot.x /= sprite_size;
  ship->pivot.y /= sprite_size;
}

void InitShip(Ship *ship, float scaler) {
  ship->angle = 0;
  ship->vel = (RL_Vector2){0, 0};

  // Ship Initial Sprite. Centered at 0,0
  ship->pivot.x = WIN_WIDTH / 2.0f;
  ship->pivot.y = WIN_HEIGHT / 2.0f;

  int sprite_size = LEN(ship->model);

  for (int i = 0; i < sprite_size; i++) {
    ship->model[i].x = ship->sprite[i].x;
    ship->model[i].y = ship->sprite[i].y;
  }

  for (int i = 0; i < sprite_size; i++) {
    ship->model[i].x += ship->pivot.x;
    ship->model[i].y += ship->pivot.y;
  }
}

void DrawShip(Ship *ship) {

  RotateShip(ship);

  if (ship->bullet.draw_flag) {
    RL_DrawRectangleV(ship->bullet.pos, (RL_Vector2){5, 5}, RL_RAYWHITE);
  }

  int sprite_size = LEN(ship->model);
  for (int i = 0; i < sprite_size; i++) {
    int j = i + 1;
    RL_DrawLineV(ship->model[i], ship->model[j % sprite_size], RL_WHITE);
  }
}

void RotateShip(Ship *ship) {

  int sprite_size = LEN(ship->model);

  for (int i = 0; i < sprite_size; i++) {
    ship->model[i] = ship->sprite[i];
  }

  for (int i = 0; i < sprite_size; i++) {

    ship->model[i] = Vector2Rotate(ship->model[i], ship->angle);
    ship->model[i] = Vector2Add(ship->model[i], ship->pivot);
  }
}
void ThrustShip(Ship *ship) {

  SetPivot(ship);
  RL_Vector2 pivotTopoint = Vector2Subtract(ship->pivot, ship->model[0]);
  pivotTopoint = Vector2Normalize(pivotTopoint);

  ship->vel.x += -pivotTopoint.x * 7.5f;

  ship->vel.y += -pivotTopoint.y * 7.5f;
}

void BoundObjectToScreen(RL_Vector2 *obj) {
  if (obj->x < 0)
    obj->x = WIN_WIDTH + obj->x;
  if (obj->x > WIN_WIDTH)
    obj->x = 0 - obj->x;
  if (obj->y < 0)
    obj->y = WIN_HEIGHT + obj->y;
  if (obj->y > WIN_HEIGHT)
    obj->y = 0 - obj->y;
}

void ShootBullet(Ship *ship) {
  SetPivot(ship);
  RL_Vector2 pivotTopoint = Vector2Subtract(ship->pivot, ship->model[0]);
  pivotTopoint = Vector2Normalize(pivotTopoint);

  ship->bullet.draw_flag = true;
  ship->bullet.pos = ship->model[0];
  ship->bullet.vel.x = pivotTopoint.x * -950;
  ship->bullet.vel.y = pivotTopoint.y * -950;
}
