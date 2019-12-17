//
// Created by roland on 2019-12-14.
//

#ifndef CATASTROPHIC_CHIP8_VM_H
#define CATASTROPHIC_CHIP8_VM_H

#include <stdint.h>
#include <stddef.h>

enum CHIP8VM_SYSCONSTANTS {
    SCR_W        = 64,
    SCR_H        = 32,
    FONTSET_SIZE = 80,
    MEM_SIZE     = 4096 // 0xFFF
};

enum CHIP8VM_SYSFLAGS{
    DRAWFLAG = 1u << 0u
};

typedef uint8_t  register8_t;
typedef uint16_t register16_t;

typedef struct _CHIP8_CPU {
    /* CHIP-8 has 16 8-bit wide registers, V0 to VF*/
    register8_t  VX[16];
    /* CHIP-8 has a 16-bit address register, I, which is 16 bits wide and is used
     * with several opcodes that involve memory operations. */
    register16_t I;

    /* CHIP-8 has two timers. They both are decremented at a rate 60 hertz, until
     * they reach 0. */
    register8_t  delay_timer;
    register8_t  sound_timer;

    /* Program counter register is 16-bit wide and contains the address of the
     * instruction being executed at the current cycle. As each instruction
     * gets fetched, the program counter is incremented by 1. */
    register16_t pc;

    /* The stack is 16 byte wide and is used to store return addresses when
     * subroutines are called. */
    register8_t  sp;
    uint16_t stack[16];

} CHIP8_CPU;


typedef struct _CHIP8_VM {
    CHIP8_CPU *cpu;

    uint8_t mem[MEM_SIZE];
    uint8_t framebuffer[SCR_W * SCR_H];
    uint8_t keypad[16]; // 16-key hexadecimal keypad

    uint16_t current_opcode;
    uint32_t cycle_cnt; // number of cpu cycles mod clock frequency
    uint32_t n_cycles_timerdecr; // cpu cycles per timer register decrement

    uint32_t flags;
} CHIP8_VM;


CHIP8_VM *CHIP8VM_init(size_t clockspeed);
void      CHIP8VM_kill(CHIP8_VM *vm);
void      CHIP8VM_load_rom(CHIP8_VM *vm, char *fpath);
void      CHIP8VM_emulate_cycle(CHIP8_VM *vm);
int       CHIP8VM_is_drawflag_set(CHIP8_VM *vm);
void      CHIP8VM_set_keys(uint8_t *keyboard_state);

#endif //CATASTROPHIC_CHIP8_VM_H
