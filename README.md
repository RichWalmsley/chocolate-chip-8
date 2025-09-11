# Chocolate CHIP8

CHIP8 emulator written in C using raylib

## How to Use

This was developed to run on linux.

1. Make sure raylib amd64 /lib and /include are installed in /usr/
2. 'sudo apt update && sudo apt install gcc gdb'
3. 'make run' to run the application
4. 'make debug' to debug the application in gdb

Currently, ROMs are loaded from the ROM folder into memory as a define at the top of main.

## TODOs

1. Find a better way to load ROM files (as arguments, add menu, etc).
2. Add sound emulation.
3. Fix emulation quirks and correct key-up press detection.