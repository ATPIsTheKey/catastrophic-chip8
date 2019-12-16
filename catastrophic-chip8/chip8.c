//
// Created by roland on 2019-12-14.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"
#include "../rf/stdutils.h"

enum CHIP8_START_ADDRS {
    RAM_START_ADDR     = 0x00,
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


CHIP8_VM*
CHIP8_init_vm(size_t clockspeed)
{
    CHIP8_VM *vm = malloc(sizeof(CHIP8_VM)); NP_CHECK(vm)
    vm->cpu = malloc(sizeof(CHIP8_CPU)); NP_CHECK(vm->cpu)

    // CPU initialization

    memset(vm->cpu->Vx, 0x00, 16 * sizeof(uint8_t));
    memset(vm->cpu->stack, 0x00, 16 * sizeof(uint8_t));

    vm->cpu->I  = 0x0000;
    vm->cpu->delay_timer = 0x00;
    vm->cpu->sound_timer = 0x00;
    vm->cpu->pc = PROGRAM_START_ADDR;

    vm->opcode    = 0x00;
    vm->cycle_cnt = 0;
    vm->n_cycles_timerdecr = (uint32_t) (clockspeed / 60);

    // System initialization

    memset(vm->mem, 0x00, MEM_SIZE * sizeof(uint8_t));
    memset(vm->framebuffer, 0x00, SCR_W * SCR_H * sizeof(uint8_t)); // clear the screen
    memcpy(vm->mem + FONTSET_START_ADDR, fontset, FONTSET_SIZE); // load the fontset
    memset(vm->keypad, 0x00, 16 * sizeof(uint8_t));

    return vm;
}

void
CHIP8_kill_vm(CHIP8_VM *vm)
{
    free(vm->cpu); vm->cpu = NULL;
    free(vm); vm = NULL;
}
