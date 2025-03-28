#include "chip8.h"

#include <stdio.h>

void chip8_init(chip8_t* chip8)
{
    memset(chip8->ram, 0, sizeof(chip8->ram));
    memset(chip8->display_buffer, 0, sizeof(chip8->display_buffer));
    memset(chip8->V_reg, 0, sizeof(chip8->V_reg));
    chip8->I_reg = 0;
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
    chip8->pc = 0x200;
    chip8->sp = 0;
    memset(chip8->stack, 0, sizeof(chip8->stack));

    // Copy fontset to 0x00
    memcpy(chip8->ram, fontset, sizeof(fontset));
}

int load_rom(chip8_t* chip8, char* filename)
{
    FILE* fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        return 0;
    }

    
}