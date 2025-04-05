#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "chip8.h"

#define ROM "roms/IBM Logo.ch8"

chip8_t emulator;
WINDOW *win;

void draw(chip8_t *chip8)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (chip8->display_buffer[x + y * WIDTH] == 0x1)
            {
                mvaddch(y + 1, x + 1, '#');
            }
            else
            {
                mvaddch(y + 1, x + 1, ' ');
            }
        }
    }
}

int main()
{
    chip8_init(&emulator);

    printf("Loading ROM file: %s\n", ROM);
    int error = load_rom(&emulator, ROM);
    switch(error)
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

    win = initscr();
    cbreak();
    noecho();
    clear();

    // Resize the initialized window and add a order
    wresize(win, HEIGHT + 2, WIDTH + 2);
    box(win, 0, 0);

    refresh();

    while(1)
    {
        chip8_cycle(&emulator);
        draw(&emulator);
        usleep(10000);
        refresh();
    }

    return 0;
}
