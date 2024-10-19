#include "raylib.h"
#include "stdlib.h"
#include "utils.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

  RL_InitWindow(DISPLAY_ROWS * DISPLAY_SCALE * 2.5, DISPLAY_COLS * DISPLAY_SCALE * 2.5,
                "Chip8-Emulator");
  RL_SetTargetFPS(60);

  float currTime = 0.0f;
  float lastInstructionTime = -100.0f;

  Chip8 *c = CHIP8_Init();
  if (c == NULL)
    return 1;


  LoadRom(argv[1], c);



  while (!RL_WindowShouldClose()) {

    currTime = RL_GetTime();

    RL_BeginDrawing();
    RL_ClearBackground(RL_BLACK);

    for (int y = 0; y < DISPLAY_COLS; y++) {
      for (int x = 0; x < DISPLAY_ROWS; x++) {

        
        if (c->Display[x][y] == 1) {
        
          RL_DrawRectangle(x * DISPLAY_SCALE * 2.5, y * DISPLAY_SCALE * 2.5, DISPLAY_SCALE * 2.5,
                           DISPLAY_SCALE * 2.5, RL_WHITE);
        }
      }
    }
    RL_EndDrawing();

    if (currTime - lastInstructionTime >= INSTRUCTION_TIME) {
      lastInstructionTime = currTime;
      CHIP8_EmulateCycle(c);
    }

    CHIP8_DecrementTimer(c, currTime);
  }

  free(c);
}
