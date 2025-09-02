#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "chip8.h"

#define ROM "roms/6-keypad.ch8"

#define ESC 27

chip8_t emulator;
uint8_t keys[16] = {0};
uint8_t keys_pressed[16] = {0};
WINDOW *win;

// TODO: Add unicode support for better visuals
void draw( chip8_t *chip8 )
{
    for ( int y = 0; y < HEIGHT; y++ )
    {
        for ( int x = 0; x < WIDTH; x++ )
        {
            mvaddch(y + 1, x + 1, chip8->display_buffer[x + y * WIDTH] ? '#' : ' ');
        }
    }
}

int map_key( int ch )
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

void poll_keys()
{
    memset( keys_pressed, 0, sizeof(keys_pressed)); // reset per-frame

    int ch;

    while ( (ch = getch()) != ERR )
    {
        int key = map_key( ch );
        if ( key != -1 )
        {
            keys_pressed[key] = 1; // key pressed this frame
        }

        if ( ch == 27 )
        {
            echo();
            endwin();
            exit(0);
        }
    }

    // Update the emulator keypad
    memcpy(keys, keys_pressed, sizeof(keys));
}

int main()
{
    chip8_init( &emulator, keys );

    printf( "Loading ROM file: %s\n", ROM );
    int error = load_rom( &emulator, ROM );
    switch( error )
    {
        case -1:
            printf( "Byte read mismatch from ROM file.\n" );
            return 0;
        
        case 0:
            printf( "File \"%s\" not found.\n", ROM );
            return 0;
        
        case 1:
            printf( "Loaded ROM file \"%s\" successfully.\n", ROM );
            break;
    }

    win = initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE); // non-blocking getch()
    keypad(stdscr, TRUE); // enable arrow keys, etc.
    clear();

    // Resize the initialized window and add a order
    wresize( win, HEIGHT + 2, WIDTH + 2 );
    box( win, 0, 0 );

    refresh();

    while(1)
    {
        poll_keys();
        chip8_cycle( &emulator );
        draw( &emulator );
        refresh();
    }

    endwin();
    return 0;
}
