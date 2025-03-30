#include "tuibox.h"
#include "chip8.h"

#define ROM "/roms/IBM Logo.ch8"

#define BLACK "\x1b[48;2;0;0;0m "
#define WHITE "\x1b[48;2;255;255;255m "
#define NEWLINE "\x1b[0m\n"

ui_t UI;
chip8_t emulator;

void text(ui_box_t *b, char *out)
{
    sprintf(out, "%s", (char*)b->data1);
}

void draw(ui_box_t *b, char *out)
{
    int x, y;

    sprintf(out, "");

    for (y = 0; y < b->h; y++)
    {
        for (x = 0; x < b->w; x++)
        {
            strcat(out, WHITE);
        }
        
        strcat(out, "\x1b[0m\n");
    }
}

void stop()
{
    ui_free(&UI);
    exit(0);
}

int main()
{
    chip8_init(&emulator);
    load_rom(&emulator, ROM);

    ui_new(0, &UI);
    
    ui_add(
        1,
        1,
        WIDTH,
        HEIGHT,
        0,
        NULL, 0,
        draw,
        NULL,
        NULL,
        NULL,
        NULL,
        &UI        
    );

    ui_key("\e", stop, &UI);

    ui_draw(&UI);

    ui_loop(&UI)
    {
        chip8_cycle(&emulator);
        ui_update(&UI);
    }

    return 0;
}
