//
// Created by roland on 2019-12-14.
//

#ifndef CATASTROPHIC_CHIP8_CHIP8_H
#define CATASTROPHIC_CHIP8_CHIP8_H

#include <stdint.h>
#include <stddef.h>

enum CHIP8_CONSTANTS {
    SCR_W        = 64,
    SCR_H        = 32,
    FONTSET_SIZE = 80,
    MEM_SIZE     = 4096 // 0xFFF
};

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


typedef struct _CHIP8_VM {
    CHIP8_CPU *cpu;

    uint16_t opcode;
    uint32_t cycle_cnt; // number of cpu cycles mod clock frequency
    uint32_t n_cycles_timerdecr; // cpu cycles per timer register decrement

    uint8_t mem[MEM_SIZE];
    uint8_t framebuffer[SCR_W * SCR_H];
    uint8_t keypad[16]; // 16-key hexadecimal keypad
} CHIP8_VM;


CHIP8_VM *CHIP8_init_vm(size_t clockspeed);
void      CHIP8_kill_vm(CHIP8_VM *vm);
void      CHIP8_load_rom(CHIP8_VM *vm, char *fpath);
void      CHIP8_emulate_cycle(CHIP8_CPU *vm);
int       CHIP8_is_drawflag_set(void);
void      CHIP8_set_keys(); // todo: some SDL keyboard state

#endif //CATASTROPHIC_CHIP8_CHIP8_H
