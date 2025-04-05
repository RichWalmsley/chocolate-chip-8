#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"

#define ROM "roms/IBM Logo.ch8"

#define BLACK "\x1b[48;2;0;0;0m "
#define WHITE "\x1b[48;2;255;255;255m "
#define NEWLINE "\x1b[0m\n"

chip8_t emulator;

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

    initscr();
    cbreak();
    noecho();
    clear();
    refresh();

    move(5,5);

    char *text = "Hello, World!";
    for (int i = 0; i < strlen(text); i++)
    {
        addch(text[i]);
        addch(' ');
    }

    refresh();

    while(1)
    {

    }

    return 0;
}
