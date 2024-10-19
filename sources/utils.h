#pragma once

#include <stdint.h>

#define DISPLAY_ROWS 64
#define DISPLAY_COLS 32
#define DISPLAY_SCALE 5
#define START_ADDR 0x200
#define FONTSET_START_ADDR 0x50
#define FONTSET_SIZE 80
#define INSTRUCTION_TIME 1.0f / 700.0f
#define OPCODE_X(opcode) ((opcode & 0x0F00) >> 8)
#define OPCODE_Y(opcode) ((opcode & 0x00F0) >> 4)
#define OPCODE_N(opcode) (opcode & 0x000F)
#define OPCODE_NN(opcode) (opcode & 0x00FF)
#define OPCODE_NNN(opcode) (opcode & 0x0FFF)

typedef struct {
  uint8_t Memory[4096];
  uint8_t Display[DISPLAY_ROWS][DISPLAY_COLS];
  uint8_t Timer;
  uint8_t SoundTimer;
  uint8_t V[16];
  uint16_t Opcode;
  uint16_t PC;
  uint16_t I;
  struct {
    uint16_t Stack[16];
    uint16_t SP;
  };
  bool Keypad[16];
} Chip8;

int LoadRom(char *romName, Chip8 *chip);

Chip8 *CHIP8_Init(void);
void CHIP8_DrawGrahpics(Chip8 *chip);
void CHIP8_EmulateCycle(Chip8 *chip );
void CHIP8_DecrementTimer(Chip8 *chip, float now);
