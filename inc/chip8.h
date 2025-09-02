#ifndef CHIP8_H_
#define CHIP8_H_

#include "stdint.h"

#define HEIGHT 32
#define WIDTH 64

typedef struct chip8
{
    uint8_t ram[4096];
    uint8_t display_buffer[HEIGHT * WIDTH];
    uint16_t pc;
    uint8_t sp;
    uint16_t I_reg;
    uint8_t V_reg[16]; // General purpose registers 0 - F. V_regf is used as flag register.
    uint16_t stack[16];
    const uint8_t* keypad; // Pointer to some keypad buffer for user input
    uint8_t delay_timer;
    uint8_t sound_timer;
} chip8_t;

void chip8_init(chip8_t* chip8, const uint8_t* keypad);
int load_rom(chip8_t* chip8, char* filename);
void chip8_cycle(chip8_t* chip8);
void print_mem(uint8_t* buffer, uint8_t height, uint8_t width);

#endif
