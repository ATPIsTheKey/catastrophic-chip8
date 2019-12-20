//
// Created by roland on 2019-12-14.
//

#ifndef CATASTROPHIC_CH8_VM_H
#define CATASTROPHIC_CH8_VM_H

#include <stdint.h>
#include <stddef.h>

enum CH8VM_SYSCONSTANTS {
    SCR_W        = 64,
    SCR_H        = 32,
    FONTSET_SIZE = 80,
    MEM_SIZE     = 4096 // 0xFFF
};

enum CH8VM_GENERAL_FLAGS {
    DRAWFLAG = 1u << 0u
};


typedef uint8_t  register8_t;
typedef uint16_t register16_t;

typedef struct _CH8_CPU {
    /* CHIP-8 has 16 8-bit wide registers, V0 to VF*/
    register8_t  Vx[16];
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
} CH8_CPU;


typedef struct _CH8_VM {
    CH8_CPU *cpu;

    uint8_t mem[MEM_SIZE];
    uint8_t framebuffer[SCR_W * SCR_H];
    uint16_t keypad[16]; // state of 16-key hexadecimal keypad

    uint16_t current_opcode;
    uint32_t cycle_cnt; // number of cpu cycles mod clock frequency
    uint32_t n_cycles_timerdecr; // cpu cycles per timer register decrement

    uint32_t flags;
} CH8_VM;


CH8_VM *CH8VM_init(size_t clockspeed);
void    CH8VM_kill(CH8_VM *vm);
void    CH8VM_load_rom(CH8_VM *vm, char *fpath);
void    CH8VM_emulate_cycle(CH8_VM *vm);
int     CH8VM_is_drawflag_set(CH8_VM *vm);
void    CH8VM_set_keys(uint8_t *keyboard_state);

#endif //CATASTROPHIC_CH8_VM_H
