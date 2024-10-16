#pragma once

#include "raylib.h"
#define WIN_WIDTH 1024
#define WIN_HEIGHT 800
#define ROTATION_SPEED 1.95
#define LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#define DEACCELARATION 2

typedef struct Bullet {
  RL_Vector2 pos;
  RL_Vector2 vel;
  bool draw_flag; 
} Bullet;
typedef struct Ship {
  RL_Vector2 pivot;
  const RL_Vector2 sprite[3];
  RL_Vector2 model[3];
  float angle;
  RL_Vector2 vel;
  Bullet bullet;
} Ship;

void InitShip(Ship *ship, float scaler);

void DrawShip(Ship *ship);

void RotateShip(Ship *ship);

void ThrustShip(Ship *ship);

void BoundObjectToScreen(RL_Vector2 *obj);

void ShootBullet(Ship *ship);
