/*******************************************************************************
 *
 * MIT License
 * Copyright (c) 2019 Roland Fuhrmann
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ******************************************************************************/

#ifndef CATASTROPHIC_CH8_VM_H
#define CATASTROPHIC_CH8_VM_H

#include <stdint.h>
#include <stddef.h>


typedef uint8_t  register8_t;
typedef uint16_t register16_t;


typedef enum {
    CH8_VM_SCR_W        = 64,
    CH8_VM_SCR_H        = 32,
    CH8_VM_FONTSET_SIZE = 80,
    CH8_VM_MEM_SIZE     = 4096, // 0xFFF
    CH8_VM_MAX_PROGSIZE = 4096 - 512
} CH8_VM_sys_constants;


typedef enum {
    CH8_VM_RAM_START_ADDR     = 0x00,  // 00
    CH8_VM_FONTSET_START_ADDR = 0x50,  // 80
    CH8_VM_PROGRAM_START_ADDR = 0x200, // 512
    CH8_VM_PROGRAM_END_ADDR  = 0xFFF  // 4095
} CH8_VM_mem_addrs;


typedef enum {
    CH8_VM_NO_OPTS = 1u << 0u,
    CH8_VM_VERBOSE_MODE = 1u << 1u,
    CH8_VM_ORIGINAL_IMPL = 1u << 2u
} CH8_VM_opt_flags;


typedef enum {
    CH8_VM_SCREEN_UPDATE = 1u << 0u
} CH8_VM_internal_flags;

typedef enum {
    CH8_VM_SUCCESS = 0,
    CH8_VM_QUIT,
    CH8_VM_RELOAD,
    CH8_VM_UNSUPPORTED_OPCODE,
    CH8_VM_ROM_NOTFOUND,
    CH8_VM_ROMSIZE_OUTOFBOUNDS,
    CH8_VM_CPU_DUMP
} CH8_VM_return_codes;


typedef struct CH8_CPU {
    // CHIP-8 has 16 8-bit wide registers, V0 to VF
    register8_t  V[16];
    // CHIP-8 has a 16-bit address register, I, which is 16 bits wide and is used
    // with several opcodes that involve memory operations. */
    register16_t I;

    // CHIP-8 has two timers. They both are decremented at a rate 60 hertz, until
    // they reach 0.
    register8_t  delay_timer;
    register8_t  sound_timer;

    // Program counter register is 16-bit wide and contains the address of the
    // instruction being executed at the current cycle. As each instruction
    // gets fetched, the program counter is incremented by 1. */
    register16_t pc;

    // The stack is 16 byte wide and is used to store return addresses when
    //subroutines are called. */
    register8_t  sp;
    uint16_t stack[16];
} CH8_CPU;


typedef struct CH8_VM {
    CH8_CPU *cpu;

    uint8_t  mem[CH8_VM_MEM_SIZE];
    uint32_t framebuffer[CH8_VM_SCR_W * CH8_VM_SCR_H]; // for convenience with SDL textures,
                                                       // framebuffer pixels are 32-bit wide
    uint8_t keypad[16]; // state of 16-key hexadecimal keypad

    uint16_t current_opcode;

    uint32_t opt_flags;
    uint32_t internal_flags;
} CH8_VM;


CH8_VM *CH8_VM_init(uint32_t opt_flags);

void    CH8_VM_kill(CH8_VM *vm);

int     CH8_VM_load_rom(CH8_VM *vm, const char *fpath);

int     CH8_VM_is_drawflag_set(CH8_VM *vm);

void    CH8_VM_unset_drawflag(CH8_VM *vm);

void    CH8_VM_decrement_timers(CH8_VM *vm);

int     CH8_VM_emulate_cycle(CH8_VM *vm);

int     CH8_VM_SDL_set_keys(CH8_VM *vm);

#endif //CATASTROPHIC_CH8_VM_H
