#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint8_t fontset[] =
    {
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

void chip8_init(chip8_t* chip8, const uint8_t* keypad)
{
    memset(chip8->ram, 0, sizeof(chip8->ram));
    memset(chip8->display_buffer, 0, sizeof(chip8->display_buffer));
    memset(chip8->V_reg, 0, sizeof(chip8->V_reg));
    chip8->I_reg = 0;
    chip8->keypad = keypad;
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
    chip8->pc = 0x200;
    chip8->sp = 0;
    memset(chip8->stack, 0, sizeof(chip8->stack));

    // Copy fontset to 0x0000
    memcpy(chip8->ram, fontset, sizeof(fontset));
}

int load_rom(chip8_t* chip8, char* filename)
{
    FILE* fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        return 0;
    }

   int bytes_read = fread(chip8->ram + 0x200, 1, sizeof(chip8->ram) - 0x200, fp);

   fseek(fp, 0L, SEEK_END);

   int fsize = ftell(fp);

   fclose(fp);

   if (bytes_read != fsize)
   {
        return -1;
   }

   return 1;
}

void chip8_cycle(chip8_t* chip8)
{
    // Fetch operation at pc location in ram. Combining two "bytes" together.
    uint16_t op = chip8->ram[chip8->pc] << 8 | chip8->ram[chip8->pc + 1];

    // Fetch x and y registers.
    uint16_t x = (op & 0x0F00) >> 8;
    uint16_t y = (op & 0x00F0) >> 4;

    switch (op & 0xF000)
    {
        case 0x0000:
            switch (op & 0x0FFF)
            {
                // CLEAR: 00E0
                // Clears the screen
                case 0x00E0:
                    memset(chip8->display_buffer, 0, sizeof(chip8->display_buffer));
                    chip8->pc += 2;
                    break;
                
                // RETURN: 00EE
                // Returns from subroutine
                case 0x00EE:
                    chip8->pc = chip8->stack[chip8->sp];
                    chip8->sp--;
                    chip8->pc += 2;
                    break;

                default:
                    printf("Opcode unknown: 0x%X.\n", op);
                    break;
            }
            break;

        // 1NNN: Jumps to address NNN
        case 0x1000:
            chip8->pc = op & 0x0FFF;
            break;

        // 2NNN: Calls subroutine at address NNN
        case 0x2000:
            chip8->sp += 1;
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->pc = op & 0x0FFF;
            break;

        // 3XNN: Skips next instruction if V_regx equals NN
        case 0x3000:
            if (chip8->V_reg[x] == (op & 0x00FF))
            {
                chip8->pc += 4;
            }
            else
            {
                chip8->pc += 2;
            }
            break;

        // 4XNN: Skips next instruction if V_regx does not equal NN
        case 0x4000:
            if (chip8->V_reg[x] != (op & 0x00FF))
            {
                chip8->pc += 4;
            }
            else
            {
                chip8->pc += 2;
            }
            break;

        // 5XY0: Skips the next instruction if V_regx equals V_regy
        case 0x5000:
            if (chip8->V_reg[x] == chip8->V_reg[y])
            {
                chip8->pc += 4;
            }
            else
            {
                chip8->pc += 2;
            }
            break;

        // 6XNN: Sets V_regx to NN
        case 0x6000:
            chip8->V_reg[x] = (op & 0x00FF);
            chip8->pc += 2;
            break;

        // 7XNN: Adds NN to V_regx
        case 0x7000:
            chip8->V_reg[x] += (op & 0x00FF);
            chip8->pc += 2;
            break;

        // 8XYn: Register operations where n = 1 - 7
        case 0x8000:
            switch (op & 0x000F)
            {
                // 8XY0: Sets V_regx to V_regy
                case 0x0000:
                    chip8->V_reg[x] = chip8->V_reg[y];
                    chip8->pc += 2;
                    break;

                // 8XY1: Sets V_regx to V_regx | V_regy
                case 0x0001:
                    chip8->V_reg[x] = (chip8->V_reg[x] | chip8->V_reg[y]);
                    chip8->V_reg[0xF] = 0;
                    chip8->pc += 2;
                    break;
                
                // 8XY2: Sets V_regx to V_regx & V_regy
                case 0x0002:
                    chip8->V_reg[x] = (chip8->V_reg[x]) & chip8->V_reg[y];
                    chip8->V_reg[0xF] = 0;
                    chip8->pc += 2;
                    break;
                
                // 8XY3: Sets V_regx to V_regx ^ V_regy
                case 0x0003:
                    chip8->V_reg[x] = (chip8->V_reg[x]) ^ chip8->V_reg[y];
                    chip8->V_reg[0xF] = 0;
                    chip8->pc += 2;
                    break;

                // 8XY4: Sets V_regx to V_regx + V_regy. Set V_regf = carry
                case 0x0004:
                    if ( chip8->V_reg[x] + chip8->V_reg[y] > 0xFF )
                    {
                        chip8->V_reg[0xF] = 1;
                        chip8->V_reg[x] = ( chip8->V_reg[x] + chip8->V_reg[y] ) & 0xFF;
                    }
                    else
                    {
                       chip8->V_reg[0xF] = 0;
                        chip8->V_reg[x] = ( chip8->V_reg[x] + chip8->V_reg[y] ) & 0xFF;
                    }

                    chip8->pc += 2;
                    break;
                
                // 8XY5: Sets V_regx to V_regx + V_regy. Set V_regf = !borrow
                case 0x0005:
                    if ( chip8->V_reg[x] > chip8->V_reg[y] )
                    {
                        chip8->V_reg[0xF] = 0;
                    }
                    else
                    {
                        chip8->V_reg[0xF] = 1;
                    }

                    chip8->V_reg[x] = chip8->V_reg[x] - chip8->V_reg[y];
                    chip8->pc += 2;
                    break;
                
                // 8XY6: Set V_regx to V_regx shifted right 1
                case 0x0006:
                    if ( chip8->V_reg[x] & 0x01 )
                    {
                        chip8->V_reg[0xF] = 1;
                    }
                    else
                    {
                        chip8->V_reg[0xF] = 0;
                    }

                    chip8->V_reg[x] /= 2;
                    chip8->pc += 2;
                    break;
                
                // 8XY7: Set V_regx to V_regy - V_regx. Set V_regf = !borrow
                case 0x0007:
                    if ( chip8->V_reg[y] > chip8->V_reg[x] )
                    {
                        chip8->V_reg[0xF] = 0; 
                    }
                    else
                    {
                        chip8->V_reg[0xF] = 1;
                    }

                    chip8->V_reg[x] = chip8->V_reg[y] - chip8->V_reg[x];
                    chip8->pc += 2;
                    break;
                
                // 8XYE: Set V_regx to V_regx shifted left 1
                case 0x000E:
                    if ( chip8->V_reg[x] & 0x80 )
                    {
                        chip8->V_reg[0xF] = 1;
                    }
                    else
                    {
                        chip8->V_reg[0xF] = 0;
                    }

                    chip8->V_reg[x] *= 2;
                    chip8->pc += 2;
                    break;
                    
                default:
                    printf("Opcode unknown: 0x%X.\n", op);
                    break;
            }
            break;

        // 9XY0: Skips the next instruction if V_regx does not equal V_regy
        case 0x9000:
            if ( chip8->V_reg[x] != chip8->V_reg[y] )
            {
                chip8->pc += 4;
            }
            else
            {
                chip8->pc += 2;
            }
            break;

        // ANNN: Sets I to address NNN
        case 0xA000:
            chip8->I_reg = (op & 0x0FFF);
            chip8->pc += 2;
            break;

        // BNNN: Jump to NNN plus V_reg0
        case 0xB000:
            chip8->pc = (op & 0x0FFF) + chip8->V_reg[0];
            break;

        // CXNN: Set V_regx to a random number AND with value NN
        case 0xC000:
            chip8->V_reg[x] = (rand() % 256) & (op & 0x00FF);
            chip8->pc += 2;
            break;

        // DXYN: Draw N pixels tall sprite from memory location that I register is holding to screen
        // V_regx holds x value and V_regy holds y value.          
        case 0xD000:
            // Extract sprite height
            uint16_t height = (op & 0x000F);

            // Temp pixel
            uint16_t px;

            // Set collision flag to 0
            chip8->V_reg[0xF] = 0;

            // For each row
            for (int d_y = 0; d_y < height; d_y++)
            {
                // Fetch pixel value from ram starting at address in I_reg
                px = chip8->ram[chip8->I_reg + d_y];

                // For each of the 8 bits in the sprite row
                for (int d_x = 0; d_x < 8; d_x++)
                {
                    // Scan through the byte and check if pixels are set
                    if ((px & (0x80 >> d_x)) != 0)
                    {
                        // If drawing erases pixels, set the collision flag
                        if (chip8->display_buffer[(chip8->V_reg[x] + d_x + ((chip8->V_reg[y] + d_y) * WIDTH))] == 1)
                        {
                            chip8->V_reg[0xF] = 1;
                        }

                        // Set pixel value of sprite at (x,y) coordinate using XOR
                        chip8->display_buffer[chip8->V_reg[x] + d_x + ((chip8->V_reg[y] + d_y) * WIDTH)] ^= 1;
                    }
                }
            }

            chip8->pc += 2;
            break;
    
        // E09E and E0A1
        case 0xE000:
            switch (op & 0x00FF)
            {
                // EX9E: Skips next instruction if the key stored in V_regx is pressed
                case 0x009E:
                    if ( chip8->keypad[chip8->V_reg[x]] != 0 )
                    {
                        chip8->pc += 4;
                    }
                    else
                    {
                        chip8->pc += 2;
                    }
                    break;
                
                // EXA1: Skips next instruction if the key stored in V_regx isn't pressed
                case 0x00A1:
                    if ( chip8->keypad[chip8->V_reg[x]] == 0 )
                    {
                        chip8->pc += 4;
                    }
                    else
                    {
                        chip8->pc += 2;
                    }
                    break;
                
                default:
                    printf("Opcode unknown: 0x%X.\n", op);
                    break;
            }
            break;

        // 0xFXNN
        case 0xF000:
            switch ( op & 0x00FF )
            {
                // FX07: Set V_regx to delay timer value
                case 0x0007:
                    chip8->V_reg[x] = chip8->delay_timer;
                    chip8->pc += 2;
                    break;
                
                // FX0A: Wait for a key press, store the value of the key in V_regx
                // TODO: Fix NOT HALTING problem
                case 0x000A:
                    int8_t key_pressed = -1;

                    for ( uint8_t i = 0; i < 16; i++)
                    {
                        if ( chip8->keypad[i] )
                        {
                            key_pressed = i;
                            break;
                        }
                    }
                    
                    if ( key_pressed == -1 )
                    {
                        return;
                    }

                    chip8->V_reg[x] = key_pressed;
                    chip8->pc += 2;
                    break;
                
                // FX15: Set delay timer to V_regx
                case 0x0015:
                    chip8->delay_timer = chip8->V_reg[x];
                    chip8->pc += 2;
                    break;
                
                // FX18: Set sound timer to V_regx
                case 0x0018:
                    chip8->sound_timer = chip8->V_reg[x];
                    chip8->pc += 2;
                    break;
                
                // FX1E: Set I_reg to I_reg + V_regx
                case 0x001E:
                    chip8->I_reg = chip8->I_reg + chip8->V_reg[x];
                    chip8->pc += 2;
                    break;
                
                // FX29: Set I_reg to the location of the sprite corresponding to the value of V_regx
                case 0x0029:
                     chip8->I_reg = chip8->V_reg[x];
                     chip8->pc += 2;
                     break;
                
                // FX33:
                case 0x0033:
                    chip8->ram[chip8->I_reg] = chip8->V_reg[x] / 100;
                    chip8->ram[chip8->I_reg + 1] = chip8->V_reg[x] / 10;
                    chip8->ram[chip8->I_reg + 2] = chip8->V_reg[x] % 10;
                    chip8->pc += 2;
                    break;

                // FX55: Copies the values of registers V_reg0 to V_regx into memory starting at the address in I_reg
                case 0x0055:
                    for ( uint8_t i = 0; i <= x; i++ )
                    {
                        chip8->ram[chip8->I_reg + i] = chip8->V_reg[i];
                    }
                    chip8->pc += 2;
                    break;
                    
                // FX65: Copies the values of from memory starting at the address in I_reg into registers V_reg0 to V_regx
                case 0x0065:
                    for ( uint8_t i = 0; i <= x; i++ )
                    {
                        chip8->V_reg[i] = chip8->ram[chip8->I_reg + i];
                    }
                    chip8->pc += 2;
                    break;
                
                default:
                    printf("Opcode unknown: 0x%X.\n", op);
                    break;
            }
            break;

        default:
            //printf("Opcode unknown: 0x%X.\n", op);
            break;
    }

    // Update timers. Decrement timers if they are greater than 0.
    if (chip8->delay_timer > 0)
    {
        chip8->delay_timer -= 1;
    }

    if (chip8->sound_timer > 0)
    {
        chip8->sound_timer -= 1;
    }
}

// Function to print memory. For debugging purposes.
void print_mem(uint8_t* buffer, uint8_t height, uint8_t width)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("%X ", buffer[i * height + j]);
        }
        printf("\n");
    }
    for (int i = 0; i < width; i++)
    {
        printf("-");
    }
    printf("\n");
}