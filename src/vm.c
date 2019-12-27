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

#include "vm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "instructions.h"
#include "debug.h"

#include "../rf/mystdlib.h"


// Original chip8 fontset (http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#font)
static uint8_t fontset[CH8_VM_FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

//  Keymap on original chip 8 machine (http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.3)
//  +---+---+---+---+
//  | 1 | 2 | 3 | C |
//  +---+---+---+---+
//  | 4 | 5 | 6 | D |
//  +---+---+---+---+
//  | 7 | 8 | 9 | E |
//  +---+---+---+---+
//  | A | 0 | B | F |
//  +---+---+---+---+
uint8_t keymap[16] = {
        SDLK_x,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_z,
        SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v,
};


//> Initializes a new chip8 vm.
CH8_VM*
CH8_VM_init(uint32_t opt_flags)
{
    CH8_VM *vm = calloc(1, sizeof(CH8_VM)); NP_CHECK(vm)
    vm->cpu = calloc(1, sizeof(CH8_CPU)); NP_CHECK(vm->cpu)
    srand(time(NULL)); // instruction Cxkk requires random numbers

    /*** CPU initialization */

    memset(vm->cpu, 0x00, 16 * sizeof(uint8_t)); // clear V registers
    memset(vm->cpu->stack, 0x00, 16 * sizeof(uint8_t)); // clear the stack

    vm->cpu->I  = 0x0000;
    vm->cpu->delay_timer = 0x00;
    vm->cpu->sound_timer = 0x00;
    vm->cpu->pc = CH8_VM_PROGRAM_START_ADDR;

    vm->current_opcode    = 0x0000;

    /*** System initialization */

    memset(vm->mem, 0x00, CH8_VM_MEM_SIZE * sizeof(uint8_t)); // clear the memory
    memset(vm->framebuffer, 0x00, CH8_VM_SCR_W * CH8_VM_SCR_H); // clear the screen
    memcpy(vm->mem + CH8_VM_FONTSET_START_ADDR, fontset, CH8_VM_FONTSET_SIZE); // load the fontset
    memset(vm->keypad, 0x00, 16 * sizeof(uint8_t)); // init keyboard

    vm->opt_flags      = 0x00 | opt_flags; // set options
    vm->internal_flags = 0x00; // used by internal functions only; should not be modified

    return vm;
}


//> deallocates dynamic memory allocated as part of vm initialization.
void
CH8_VM_kill(CH8_VM *vm)
{
    free(vm->cpu); vm->cpu = NULL;
    free(vm); vm = NULL;
}


//> Loads a compatible rom into chip8 memory.
int
CH8_VM_load_rom(CH8_VM *vm, const char *fpath)
{
    FILE *rom_fp = fopen(fpath, "rb");
    if (rom_fp == NULL) {
        CH8_VM_DBG_log(__func__,
                "Rom could not be opened. Terminate execution.\n");
        return CH8_VM_ROM_NOTFOUND;
    }

    // get rom size
    fseek(rom_fp, 0L, SEEK_END);
    size_t sz = ftell(rom_fp);
    rewind(rom_fp);

    // make sure rom fits into memory
    if (sz > CH8_VM_MAX_PROGSIZE) {
        CH8_VM_DBG_log(__func__,
                "Rom size out of bounds: %zu bytes (max is %zu). Terminate execution.\n",
                sz, CH8_VM_MAX_PROGSIZE);
        return CH8_VM_ROMSIZE_OUTOFBOUNDS;
    }

    // read rom into memory from file
    uint8_t b;
    for (int i = 0; fread(&b, sizeof(b), 1, rom_fp) != 0; i++)
    {
        if (!feof(rom_fp))
            vm->mem[CH8_VM_PROGRAM_START_ADDR + i] = b;
    }
    return CH8_VM_SUCCESS;
}


//> Returns whether drawflag is set and framebuffer has to be redrawn.
inline int
CH8_VM_is_drawflag_set(CH8_VM *vm)
{
    return vm->internal_flags & CH8_VM_SCREEN_UPDATE ? 1 : 0;
}


//> Unsets drawflag of vm.
void
CH8_VM_unset_drawflag(CH8_VM *vm)
{
    vm->internal_flags &= ~CH8_VM_SCREEN_UPDATE;
}


//> Decrements timers if they are set.
void
CH8_VM_decrement_timers(CH8_VM *vm)
{
    if (vm->cpu->delay_timer > 0) { vm->cpu->delay_timer--; }
    if (vm->cpu->sound_timer > 0) { vm->cpu->sound_timer--; }
}


//> Emulates a single CPU cycle
int
CH8_VM_emulate_cycle(CH8_VM *vm)
{
    int rc;

    // decode the instruction
    vm->current_opcode = vm->mem[vm->cpu->pc] << 8 | vm->mem[vm->cpu->pc + 1];
    // execute the instruction
    rc = CH8_INSTR_exec(vm);
    // increment the program counter to get next instruction
    vm->cpu->pc += 2;
    return rc;
}


//> Sets keypad buffer according to current keyboard state and returns event codes if
//  special keys have been pressed.
int
CH8_VM_SDL_set_keys(CH8_VM *vm)
{
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            return CH8_VM_QUIT;

        // process keydown events
        if (e.type == SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_ESCAPE)
                return CH8_VM_QUIT;

            if (e.key.keysym.sym == SDLK_F1)
                return CH8_VM_RELOAD;

            if (e.key.keysym.sym == SDLK_F2)
                return CH8_VM_CPU_DUMP;

            for (int i = 0; i < 16; ++i)
                if (e.key.keysym.sym == keymap[i])
                    vm->keypad[i] = 1u; // set key states
        }

        // process keyup events
        if (e.type == SDL_KEYUP)
        {
            for (int i = 0; i < 16; ++i)
                if (e.key.keysym.sym == keymap[i])
                    vm->keypad[i] = 0u; // unset key states
        }
    }
    return CH8_VM_SUCCESS;
}
