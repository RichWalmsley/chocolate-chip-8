#ifndef CHIP8_H_
#define CHIP8_H_

#include "stdint.h"

typedef struct chip8
{
    char* filename;
    uint8_t ram[4096];
    uint8_t display_buffer[256];
    uint16_t pc;
    uint8_t sp;
    uint16_t I_reg;
    uint16_t op;
    uint8_t V_reg[16];
    uint16_t stack[16];
    uint8_t delay_timer;
    uint8_t sound_timer;
} chip8_t;

void chip8_init(chip8_t* chip8);
int load_rom(chip8_t* chip8, char* filename);
void chip8_fetch(chip8_t* chip8);
void chip8_decode(chip8_t* chip8);
void chip8_execute(chip8_t* chip8);
void chip8_cycle(chip8_t* chip8);

#endif
