#ifndef CHIP8_H
#define CHIP8_H

#include "stdint.h"

enum INST
{
    //TODO: Implement instruction list
};

typedef struct
{
    uint8_t ram[4096];
    uint8_t display_buffer[256];
    uint16_t* pc;
    uint16_t* I_reg;
    uint16_t stack[16];
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t V0_reg;
    uint8_t V1_reg;
    uint8_t V2_reg;
    uint8_t V3_reg;
    uint8_t V4_reg;
    uint8_t V5_reg;
    uint8_t V6_reg;
    uint8_t V7_reg;
    uint8_t V8_reg;
    uint8_t V9_reg;
    uint8_t VA_reg;
    uint8_t VB_reg;
    uint8_t VC_reg;
    uint8_t VD_reg;
    uint8_t VE_reg;
    uint8_t VF_reg;
} chip8_t;

void chip8_init(chip8_t chip8);
void chip8_update(chip8_t chip8);
void chip8_fetch(chip8_t chip8);
void chip8_decode(chip8_t chip8);
void chip8_execute(chip8_t chip8);

#endif
