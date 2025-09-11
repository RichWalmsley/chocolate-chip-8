#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "raylib.h"
#include "chip8.h"

#define ROM "roms/glitchGhost.ch8"

#define ESC 27

chip8_t emulator;

void draw(chip8_t* chip8)
{
    BeginDrawing();

    ClearBackground( BLACK );

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (chip8->display_buffer[x + (WIDTH * y)])
            {
                DrawRectangle(16 * x, 16 * y, 16, 16, BLUE);
            }
        }
    }

    EndDrawing();
}

void handle_keys(chip8_t* chip8)
{
    chip8->keypad[0x1] = IsKeyDown(KEY_ONE);
    chip8->keypad[0x2] = IsKeyDown(KEY_TWO);
    chip8->keypad[0x3] = IsKeyDown(KEY_THREE);
    chip8->keypad[0xC] = IsKeyDown(KEY_FOUR);

    chip8->keypad[0x4] = IsKeyDown(KEY_Q);
    chip8->keypad[0x5] = IsKeyDown(KEY_W);
    chip8->keypad[0x6] = IsKeyDown(KEY_E);
    chip8->keypad[0xD] = IsKeyDown(KEY_R);

    chip8->keypad[0x7] = IsKeyDown(KEY_A);
    chip8->keypad[0x8] = IsKeyDown(KEY_S);
    chip8->keypad[0x9] = IsKeyDown(KEY_D);
    chip8->keypad[0xE] = IsKeyDown(KEY_F);

    chip8->keypad[0xA] = IsKeyDown(KEY_Z);
    chip8->keypad[0x0] = IsKeyDown(KEY_X);
    chip8->keypad[0xB] = IsKeyDown(KEY_C);
    chip8->keypad[0xF] = IsKeyDown(KEY_V);
}

int map_key(int ch)
{
    switch (ch)
    {
        case 'x':
            return 0x0;
        case '1':
            return 0x1;
        case '2':
            return 0x2;
        case '3':
            return 0x3;
        case 'q':
            return 0x4;
        case 'w':
            return 0x5;
        case 'e':
            return 0x6;
        case 'a':
            return 0x7;
        case 's':
            return 0x8;
        case 'd':
            return 0x9;
        case 'z':
            return 0xA;
        case 'c':
            return 0xB;
        case '4':
            return 0xC;
        case 'r':
            return 0xD;
        case 'f':
            return 0xE;
        case 'v':
            return 0xF;
        default:
            return -1;
    }
}

int main()
{
    chip8_init(&emulator);

    printf("Loading ROM file: %s\n", ROM);
    int error = load_rom(&emulator, ROM);
    switch( error )
    {
        case -1:
            printf("Byte read mismatch from ROM file.\n");
            return 0;
        
        case 0:
            printf("File \"%s\" not found.\n", ROM);
            return 0;
        
        case 1:
            printf("Loaded ROM file \"%s\" successfully.\n", ROM);
            break;
    }

    const int screenWidth = WIDTH * 16;
    const int screenHeight = HEIGHT * 16;
    const int INSTRUCTIONS_PER_FRAME = 20;

    InitWindow(screenWidth, screenHeight, "Chocolate CHIP 8");
    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        handle_keys(&emulator);

        // Run several instructions per frame
        for (int i = 0; i < INSTRUCTIONS_PER_FRAME; i++)
        {
            chip8_cycle(&emulator);
        }

        // Decoupled timers to tick at 60Hz. 
        if (emulator.delay_timer > 0)
        {
            emulator.delay_timer--;
        }

        if (emulator.sound_timer > 0)
        {
            // TODO: Add beep
            emulator.sound_timer--;
        }

        draw(&emulator);
    }

    CloseWindow();

    return 0;
}
