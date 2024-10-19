#include "utils.h"
#include "math.h"
#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const uint8_t FONTSET[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
int LoadRom(char *romName, Chip8 *chip) {
  // FILE* rom = fopen(filename, "r");
  FILE *rom = NULL;
  int err = fopen_s(&rom, romName, "rb");
  if (err != 0) {
    printf("ERROR!");
    return err;
  }
  fseek(rom, 0, SEEK_END);
  long length = ftell(rom);
  if (length <= 0) {
    fclose(rom);
    return 1;
  }
  rewind(rom);
  // fseek(rom, 0, SEEK_SET);

  // integer overflow maybe?
  uint8_t *data = calloc(length + 1, sizeof(uint8_t));
  if (data == NULL) {
    fclose(rom);
    return 1;
  };

  fread(data, sizeof(uint8_t), length, rom);

  for (int i = 0; i <= length; i++) {
    chip->Memory[START_ADDR + i] = data[i];
  }

  fclose(rom);
  free(data);

  return 0;
}
bool KeyPressed(Chip8 *chip) {
  // Key mappings for Chip-8 keypad
  const int keyMap[16] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_Q, KEY_W,
                          KEY_E,   KEY_R,   KEY_A,     KEY_S,    KEY_D, KEY_F,
                          KEY_Z,   KEY_X,   KEY_C,     KEY_V};

  for (int i = 0; i < 16; i++) {
    if (RL_IsKeyDown(keyMap[i])) {
      chip->Keypad[i] = true;
      return true; // A key is pressed
    }
  }
  return false; // No keys pressed
}

Chip8 *CHIP8_Init(void) {
  Chip8 *chip = (Chip8 *)malloc(sizeof(Chip8));
  if (chip == NULL) {
    return NULL;
  }
  memset(chip->Memory, 0, sizeof(chip->Memory));
  memset(chip->Display, 0, sizeof(chip->Display));
  memset(chip->Stack, 0, sizeof(chip->Stack));
  memset(chip->V, 0, sizeof(chip->V));

  chip->SP = 0;
  chip->PC = START_ADDR;

  for (int i = 0; i < FONTSET_SIZE; i++) {
    chip->Memory[FONTSET_START_ADDR + i] = FONTSET[i];
  }

  return chip;
}
void CHIP8_EmulateCycle(Chip8 *chip) {
  // Fetch

  chip->Opcode = (chip->Memory[chip->PC] << 8) | chip->Memory[chip->PC + 1];
  chip->PC += 2;

  // Decode
  uint8_t X = OPCODE_X(chip->Opcode);
  uint8_t Y = OPCODE_Y(chip->Opcode);
  uint8_t N = OPCODE_N(chip->Opcode);
  uint8_t NN = OPCODE_NN(chip->Opcode);
  uint16_t NNN = OPCODE_NNN(chip->Opcode);

  // Execute
  switch ((chip->Opcode & 0xF000) >> 12) {
  case 0x0:
    switch (N) {
    case 0x0: // 0x00E0
      for (int i = 0; i < DISPLAY_ROWS; i++) {
        for (int j = 0; j < DISPLAY_COLS; j++) {
          chip->Display[i][j] = 0;
        }
      }
      break;
    case 0xE: // 0x00EE
      chip->SP--;
      chip->PC = chip->Stack[chip->SP];

      break;
    }

    break;

  case 0x1: // 1NNN
    chip->PC = NNN;
    break;

  case 0x2: // 2NNN
    chip->Stack[chip->SP++] = chip->PC; // Save return address

    chip->PC = NNN; // Jump to subroutine at NNN
    break;
    
    break;

  case 0x3: // 3XNN
    if (chip->V[X] == NN)
      chip->PC += 2;
    break;
  case 0x4: // 4XNN
    if (chip->V[X] != NN)
      chip->PC += 2;
    break;
  case 0x5: // 5XY0
    if (chip->V[X] == chip->V[Y])
      chip->PC += 2;
    break;
  case 0x6: // 6XNN
    chip->V[X] = NN;
    break;
  case 0x7: // 7XNN
    chip->V[X] += NN;
    break;
  case 0x8: // 8XYN
    switch (N) {
    case 0x0:
      chip->V[X] = chip->V[Y];
      break;

    case 0x1:
      chip->V[X] |= chip->V[Y];
      break;
    case 0x2:
      chip->V[X] &= chip->V[Y];
      break;
    case 0x3:
      chip->V[X] ^= chip->V[Y];
      break;
    case 0x4:
      chip->V[X] += chip->V[Y];
      if (chip->V[X] >= 255)
        chip->V[0xF] = 1;
      break;
    case 0x5:
      if (chip->V[X] >= chip->V[Y])
        chip->V[0xF] = 1;
      else
        chip->V[0xF] = 0;
      chip->V[X] -= chip->V[Y];
      break;
    case 0x6:
      // chip->V[X] = chip->V[Y];
      chip->V[0xF] = chip->V[X] & 0x1;
      chip->V[X] >>= 1;
      break;
    case 0x7:
      if (chip->V[Y] >= chip->V[X])
        chip->V[0xF] = 1;
      else
        chip->V[0xF] = 0;
      chip->V[X] -= chip->V[Y] * -1;
      break;

    case 0xE:
      // chip->V[X] = chip->V[Y];
      chip->V[0xF] = (chip->V[X] >> 7) & 1;
      chip->V[X] <<= 1;
      break;
    }
    break;

  case 0x9: // 9XY0
    if (chip->V[X] != chip->V[Y])
      chip->PC += 2;
  case 0xA: /// ANNN
    chip->I = NNN;
    printf("I set to: 0x%03X\n", chip->I);
    break;
  case 0xB: // BNNN
    chip->I = NNN + chip->V[0];
    break;
  case 0xC: // CXNN
    int n = rand() % 256;
    chip->V[X] = n & NN;
    printf("%i", chip->V[X]);
    break;
  case 0xD: // DXYN

    uint8_t spritePosX = chip->V[X] % DISPLAY_ROWS;
    uint8_t spritePosY = chip->V[Y] % DISPLAY_COLS;

    chip->V[0xF] = 0;

    for (int i = 0; i < N; i++) {
      uint8_t spriteData = chip->Memory[chip->I + i];
      uint8_t pixelPosY = spritePosY + i;
      if (pixelPosY >= DISPLAY_COLS)
        break;
      for (int j = 0; j < 8; j++) {
        bool isPixel = (spriteData >> (7 - j)) & 1;
        uint8_t pixelPosX = spritePosX + j;

        if (pixelPosX >= DISPLAY_ROWS)
          break;

        if (isPixel) {
          if (chip->Display[pixelPosX][pixelPosY]) {
            chip->Display[pixelPosX][pixelPosY] = 0;
            chip->V[0xF] = 1;
          } else
            chip->Display[pixelPosX][pixelPosY] = 1;
        }
      }
    }

    break;
  case 0xE:
    switch (N) {
    case 0xE: // EX9E
      if (chip->Keypad[chip->V[X]])
        chip->PC += 2;
      break;
    case 0x1: // EXA1
      if (!chip->Keypad[chip->V[X]])
        chip->PC += 2;
      break;
    }
    break;

  case 0xF:
    switch (Y) {
    case 0x5: // FX55
      for (int i = 0; i <= X; i++) {
        chip->Memory[chip->I + i] = chip->V[i];
      }
      break;
    case 0x6: // FX65
      for (int i = 0; i <= X; i++) {
        chip->V[i] = chip->Memory[chip->I + i];
      }

      break;
    }

    switch (N) {

    case 0x3: // FX33

      for (int i = 3; i >= 1; i--) {
        int digit = (chip->V[X] / (int)pow(10, i - 1)) % 10;
        chip->Memory[chip->I + (3 - i)] = digit;
      }
      break;

    case 0x7: // FX07
      chip->V[X] = chip->Timer;
      break;
    case 0x5: // FX15
      chip->Timer = chip->V[X];
      break;
    case 0x8: // FX18
      chip->SoundTimer = chip->V[X];
      break;
    case 0x9: // FX29
      chip->I = FONTSET_START_ADDR + (chip->V[X] * 5);

      break;
    case 0xA: // FX0A
      if (!KeyPressed(chip)) {
        chip->PC -= 2;
      } else {
        for (int i = 0; i < 16; i++) {
          if (chip->Keypad[i]) {
            chip->V[X] = i;
            chip->Keypad[i] = false;
            break;
          }
        }
      }

      break;
    case 0xE: // FX1E
      chip->I += chip->V[X];
      if (chip->I > 0x1000) {
        chip->V[0xF] = 1;
      }
      break;
    }
    break;
  }
}

void CHIP8_DecrementTimer(Chip8 *chip, float now) {
  static float lastInstructionTime = 0;

  if (now - lastInstructionTime >= (1.0f / 60.0f)) {
    lastInstructionTime = now;
    chip->Timer--;
    chip->SoundTimer--;
  }
}
