//
// Created by roland on 2019-12-14.
//

#ifndef CATASTROPHIC_CHIP8_CHIP8_H
#define CATASTROPHIC_CHIP8_CHIP8_H

#include <stdint.h>


typedef uint8_t  register8_t;
typedef uint16_t register16_t;


typedef struct _CHIP8_CPU {
    /* CHIP-8 has 16 8-bit data registers named V0 to VF. The VF register doubles as
     * a flag for some instructions; thus, it should be avoided. In an addition
     * operation, VF is the carry flag, while in subtraction, it is the "no borrow"
     * flag. In the draw instruction VF is set upon pixel collision.
     * (https://en.wikipedia.org/wiki/CHIP-8#Registers) */
    register8_t  Vx[16];

    /* The address register, which is named I, is 16 bits wide and is used with
     * several opcodes that involve memory operations.
     * (https://en.wikipedia.org/wiki/CHIP-8#Registers) */
    register16_t I;

    /* CHIP-8 has two timers. They both count down at 60 hertz, until they reach 0.
     * • Delay timer: This timer is intended to be used for timing the events of
     *   games. Its value can be set and read.
     * • Sound timer: This timer is used for sound effects. When its value is
     *   nonzero, a beeping sound is made.
     * (https://en.wikipedia.org/wiki/CHIP-8#Timers) */
    register8_t  delay_timer;
    register8_t  sound_timer;

    /* A program counter is a register that contains the address of the instruction
     * being executed at the current time. As each instruction gets fetched, the
     * program counter increases its stored value by 1. After each instruction is
     * fetched, the program counter points to the next instruction in the sequence.
     * When the computer restarts or is reset, the program counter normally reverts
     * to 0. */
    register16_t pc;

    /* The stack is used to store return addresses when subroutines are called. */
    register8_t  sp;
    uint16_t stack[16];
} CHIP8_CPU;


typedef struct _CHIP8_SYS {
    CHIP8_CPU *cpu;

    /* Memory Map:
     *  +---------------+= 0xFFF (4095) End of Chip-8 RAM
     *  | 0x200 to 0xFFF|
     *  |     Chip-8    |
     *  | Program / Data|
     *  |     Space     |
     *  +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
     *  |               |
     *  +---------------+= 0x200 (512) Start of most Chip-8 programs
     *  | 0x000 to 0x1FF|
     *  | Reserved for  |
     *  |  interpreter  |
     *  +---------------+= 0x000 (0) Start of Chip-8 RAM
     *  (http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.1)
     */
    uint8_t mem[4096];

    /* Original CHIP-8 Display resolution is 64×32 pixels, and color is monochrome.
     * Graphics are drawn to the screen solely by drawing sprites, which are 8 pixels
     * wide and may be from 1 to 15 pixels in height. Sprite pixels are XOR'd with
     * corresponding screen pixels.
     * (https://en.wikipedia.org/wiki/CHIP-8#Graphics_and_sound) */
    uint8_t framebuffer[64*32];

    /* Chip8 comes with a font set (sprites) that allows character 0-9 and A-F to be
     * printed directly to the screen.Each one of these characters fit within a 8x5
     * grid (all font data stored in first 80 bytes of memory).
     * (http://www.cs.columbia.edu/~sedwards/classes/2016/4840-spring/designs/Chip8.pdf) */
    uint8_t fontbuffer[16*5];

    /* Input is done with a hex keyboard that has 16 keys ranging 0 to F.
     * The '8', '4', '6', and '2' keys are typically used for directional input.
     * Three opcodes are used to detect input. One skips an instruction if a specific
     * key is pressed, while another does the same if a specific key is not pressed.
     * The third waits for a key press, and then stores it in one of the data
     * registers.
     * (https://en.wikipedia.org/wiki/CHIP-8#Input) */
    uint8_t keypad[16];
} CHIP8_SYS;


CHIP8_SYS *CHIP8_sysinit();
void CHIP8_syskill(CHIP8_SYS *sys);

#endif //CATASTROPHIC_CHIP8_CHIP8_H
