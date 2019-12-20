//
// Created by roland on 2019-12-14.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"
#include "../rf/stdutils.h"

enum CHIP8VM_ADDRS {
    RAM_START_ADDR     = 0x00, // 00
    FONTSET_START_ADDR = 0x50, // 80
    PROGRAM_START_ADDR = 0x200 // 200
};

static uint8_t fontset[FONTSET_SIZE] ={
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


CH8_VM*
CHIP8VM_init(size_t clockspeed)
{
    CH8_VM *vm = malloc(sizeof(CH8_VM)); NP_CHECK(vm)
    vm->cpu = malloc(sizeof(CH8_CPU)); NP_CHECK(vm->cpu)

    // CPU initialization

    memset(vm->cpu->Vx, 0x00, 16 * sizeof(uint8_t));
    memset(vm->cpu->stack, 0x00, 16 * sizeof(uint8_t));

    vm->cpu->I  = 0x0000;
    vm->cpu->delay_timer = 0x00;
    vm->cpu->sound_timer = 0x00;
    vm->cpu->pc = PROGRAM_START_ADDR;

    vm->current_opcode    = 0x00;
    vm->cycle_cnt = 0;
    vm->n_cycles_timerdecr = (uint32_t) (clockspeed / 60);

    // System initialization

    memset(vm->mem, 0x00, MEM_SIZE * sizeof(uint8_t));
    memset(vm->framebuffer, 0x00, SCR_W * SCR_H * sizeof(uint8_t)); // clear the screen
    memcpy(vm->mem + FONTSET_START_ADDR, fontset, FONTSET_SIZE); // load the fontset
    memset(vm->keypad, 0x00, 16 * sizeof(uint8_t));

    vm->flags = 0x00;

    return vm;
}


void
CHIP8VM_kill(CH8_VM *vm)
{
    free(vm->cpu); vm->cpu = NULL;
    free(vm); vm = NULL;
}


void
CHIP8VM_load_rom(CH8_VM *vm, char *fpath)
{
    FILE *rom_fp = fopen(fpath, "rb"); NP_CHECK(rom_fp)

    uint8_t instr;
    for (int i = 0; fread(&instr, sizeof(instr), 1, rom_fp) != 0; i++)
        vm->mem[PROGRAM_START_ADDR + i] = instr;
}


static inline void
set_flag(CH8_VM *vm, uint32_t flag)
{
    vm->flags |= flag;
}


static inline void
unset_flag(CH8_VM *vm, uint32_t flag)
{
    vm->flags &= ~flag;
}


int
CHIP8VM_is_drawflag_set(CH8_VM *vm)
{
    return vm->flags & DRAWFLAG ? 1 : 0;
}


void
CHIP8VM_emulate_cycle(CH8_VM *vm)
{
    uint16_t opcode = vm->mem[vm->cpu->pc] <<  8 | vm->mem[vm->cpu->pc + 1];

    vm->current_opcode = opcode;
    switch (opcode & 0xF000) {
        default:
            break;
    }

    vm->cpu->pc += 2;
}
