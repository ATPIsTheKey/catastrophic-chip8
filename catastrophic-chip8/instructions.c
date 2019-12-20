#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc32-c"
#pragma ide diagnostic ignored "cert-msc30-c"
//
// Created by roland on 2019-12-17.
//

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "instructions.h"

/* > Execute machine language subroutine at address nnn. */
void 
CH8INSTR_0nnn(CH8_VM *vm)
{}


/* > Clear the screen. */
void 
CH8INSTR_00E0(CH8_VM *vm)
{
    memset(vm->framebuffer, 0x00, sizeof(vm->framebuffer));
}


/* > Return from a subroutine. */
void 
CH8INSTR_00EE(CH8_VM *vm)
{
    vm->cpu->pc = vm->cpu->stack[vm->cpu->sp];
    vm->cpu->sp--;
}


/* > Jump to address nnn. */
void 
CH8INSTR_1nnn(CH8_VM *vm)
{
    uint16_t nnn = (vm->current_opcode & 0x0FFFu);

    vm->cpu->pc = nnn;
}


/* > Execute subroutine starting at address nnn. */
void 
CH8INSTR_2nnn(CH8_VM *vm)
{
    uint16_t nnn = (vm->current_opcode & 0x0FFFu);

    vm->cpu->sp++;
    vm->cpu->stack[vm->cpu->sp] = vm->cpu->pc;
    vm->cpu->pc = nnn;
}


/* > Skip the following instruction if the value of register Vx equals KK. */
void 
CH8INSTR_3xKK(CH8_VM *vm)
{
    uint8_t x  = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t KK = (vm->current_opcode & 0x00FFu);

    if (vm->cpu->Vx[x] == KK)
        vm->cpu->pc += 2;
}


/* > Skip the following instruction if the value of register Vx is not equal to KK. */
void 
CH8INSTR_4xKK(CH8_VM *vm)
{
    uint8_t x  = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t KK = (vm->current_opcode & 0x00FFu);

    if (vm->cpu->Vx[x] != KK)
        vm->cpu->pc += 2;
}


/* > Skip the following instruction if the value of register Vx is equal to the value
 *   of register VY. */
void 
CH8INSTR_5xY0(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t y = (vm->current_opcode & 0x0F00u) >> 4u;

    if (vm->cpu->Vx[x] == vm->cpu->Vx[y])
        vm->cpu->pc += 2;
}


/* > Store number KK in register Vx. */
void 
CH8INSTR_6xKK(CH8_VM *vm)
{
    uint8_t x  = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t KK = (vm->current_opcode & 0x00FFu);

    vm->cpu->Vx[x] = KK;
}


/* > Add the value KK to register Vx. */
void 
CH8INSTR_7xKK(CH8_VM *vm)
{
    uint8_t x  = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t KK = (vm->current_opcode & 0x00FFu);

    vm->cpu->Vx[x] += KK;
}


/* > Store the value of register Vy in register Vx. */
void 
CH8INSTR_8xy0(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t y = (vm->current_opcode & 0x00F0u) >> 4u;

    vm->cpu->Vx[x] = vm->cpu->Vx[y];
}


/* > Set Vx to Vx OR Vy. */
void 
CH8INSTR_8xy1(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t y = (vm->current_opcode & 0x00F0u) >> 4u;

    vm->cpu->Vx[x] |= vm->cpu->Vx[y];
}


/* > Set Vx to Vx AND Vy. */
void 
CH8INSTR_8xy2(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t y = (vm->current_opcode & 0x00F0u) >> 4u;

    vm->cpu->Vx[x] &= vm->cpu->Vx[y];
}


/* > Set Vx to Vx xOR Vy. */
void 
CH8INSTR_8xy3(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t y = (vm->current_opcode & 0x00F0u) >> 4u;

    vm->cpu->Vx[x] ^= vm->cpu->Vx[y];
}


/* > Add the value of register Vy to register Vx.
 * > Set VF to 01 if a carry occurs.
 * > Set VF to 00 if a carry does not occur. */
void 
CH8INSTR_8xy4(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t y = (vm->current_opcode & 0x00F0u) >> 4u;

    uint16_t r = vm->cpu->Vx[x] + vm->cpu->Vx[y];
    if (r > 255u) {
        vm->cpu->Vx[0x0F] = 1u;
    } else {
        vm->cpu->Vx[0x0F] = 0u;
    }

    vm->cpu->Vx[x] = r & 0x00FFu;
}


/* > Subtract the value of register Vy from register Vx
 * > Set VF to 00 if a borrow occurs
 * > Set VF to 01 if a borrow does not occur */
void 
CH8INSTR_8xy5(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t y = (vm->current_opcode & 0x00F0u) >> 4u;

    if (vm->cpu->Vx[x] > vm->cpu->Vx[y]) {
        vm->cpu->Vx[0x0F] = 1u;
    } else {
        vm->cpu->Vx[0x0F] = 0u;
    }

    vm->cpu->Vx[x] -= vm->cpu->Vx[y];
}


/* > Store the value of register Vy shifted right one bit in register Vx.
 * > Set register VF to the least significant bit prior to the shift.
 * > Vy is unchanged. */
void 
CH8INSTR_8xy6(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;

    vm->cpu->Vx[0x0F] = (vm->cpu->Vx[x] & 0x01u);
    vm->cpu->Vx[x] >>= 1u;
}


/* > Set register Vx to the value of Vy minus Vx.
 * > Set VF to 00 if a borrow occurs.
 * > Set VF to 01 if a borrow does not occur. */
void 
CH8INSTR_8xy7(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t y = (vm->current_opcode & 0x00F0u) >> 4u;

    if (vm->cpu->Vx[y] > vm->cpu->Vx[x]) {
        vm->cpu->Vx[0x0F] = 1u;
    } else {
        vm->cpu->Vx[0x0F] = 0u;
    }

    vm->cpu->Vx[x] = vm->cpu->Vx[y] - vm->cpu->Vx[x];
}


/* > Shifts the value of register Vx to the left one bit.
 * > Set register VF to the most significant bit prior to the shift.
 * > Vy is unchanged. */
void 
CH8INSTR_8xyE(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;

    vm->cpu->Vx[0x0F] = (vm->cpu->Vx[x] & 0x80u) >> 7u;
    vm->cpu->Vx[x] <<= 1u;
}


/* > Skip the following instruction if the value of register Vx is not equal to the
 * > value of register Vy. */
void 
CH8INSTR_9xy0(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t y = (vm->current_opcode & 0x00F0u) >> 4u;

    if (vm->cpu->Vx[x] != vm->cpu->Vx[y])
        vm->cpu->pc += 2;
}


/* > Store memory address nnn in register I. */
void 
CH8INSTR_Annn(CH8_VM *vm)
{
    uint16_t nnn = (vm->current_opcode & 0x0FFFu);

    vm->cpu->I = nnn;
}


/* > Jump to address nnn + V0. */
void 
CH8INSTR_Bnnn(CH8_VM *vm)
{
    uint16_t nnn = (vm->current_opcode & 0x0FFFu);

    vm->cpu->pc = nnn + vm->cpu->Vx[0x00];
}


/* > Set Vx to a random number with a mask of KK. */
void 
CH8INSTR_CxKK(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t KK = (vm->current_opcode & 0x00FFu);

    srand(time(NULL));
    vm->cpu->Vx[x] = (uint8_t)(rand() % (255u + 1u)) & KK;
}


/* > Draw a sprite at position Vx, Vy with N bytes of sprite data starting at the
 *   address stored in I.
 * > Set VF to 01 if any set pixels are changed to unset, and 00 otherwise. */
void 
CH8INSTR_DxyN(CH8_VM *vm)
{}


/* > Skip the following instruction if the key corresponding to the hex value 
 *   currently stored in register Vx is pressed */
void 
CH8INSTR_Ex9E(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;

    if (vm->keypad[x])
        vm->cpu->pc += 2;
}


/* > Skip the following instruction if the key corresponding to the hex value 
 *   currently stored in register Vx is not pressed*/
void 
CH8INSTR_ExA1(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    
    if (!vm->keypad[x])
        vm->cpu->pc += 2;
}


/* > Store the current value of the delay timer in register Vx. */
void 
CH8INSTR_Fx07(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;

    vm->cpu->Vx[x] = vm->cpu->delay_timer;
}


/* > Wait for a keypress and store the result in register Vx. */
void 
CH8INSTR_Fx0A(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;
    
    if (vm->keypad[0])
        vm->cpu->Vx[x] = 0x00;
    else if (vm->keypad[1])
        vm->cpu->Vx[x] = 0x01;
    else if (vm->keypad[2])
        vm->cpu->Vx[x] = 0x02;
    else if (vm->keypad[3])
        vm->cpu->Vx[x] = 0x03;
    else if (vm->keypad[4])
        vm->cpu->Vx[x] = 0x04;
    else if (vm->keypad[5])
        vm->cpu->Vx[x] = 0x05;
    else if (vm->keypad[6])
        vm->cpu->Vx[x] = 0x06;
    else if (vm->keypad[7])
        vm->cpu->Vx[x] = 0x07;
    else if (vm->keypad[8])
        vm->cpu->Vx[x] = 0x08;
    else if (vm->keypad[9])
        vm->cpu->Vx[x] = 0x09;
    else if (vm->keypad[10])
        vm->cpu->Vx[x] = 0x0A;
    else if (vm->keypad[11])
        vm->cpu->Vx[x] = 0x0B;
    else if (vm->keypad[12])
        vm->cpu->Vx[x] = 0x0C;
    else if (vm->keypad[13])
        vm->cpu->Vx[x] = 0x0D;
    else if (vm->keypad[14])
        vm->cpu->Vx[x] = 0x0E;
    else if (vm->keypad[15])
        vm->cpu->Vx[x] = 0x0F;
    else
        vm->cpu->pc -= 2;
}


/* > Set the delay timer to the value of register Vx. */
void 
CH8INSTR_Fx15(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;

    vm->cpu->delay_timer = x;
}


/* > Set the sound timer to the value of register Vx. */
void 
CH8INSTR_Fx18(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;

    vm->cpu->delay_timer = x;
}


/* > Add the value stored in register Vx to register I. */
void 
CH8INSTR_Fx1E(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;

    vm->cpu->I += vm->cpu->Vx[x];
}


/* > Set I to the memory address of the sprite data corresponding to the hexadecimal
 *   digit stored in register Vx. */
void 
CH8INSTR_Fx29(CH8_VM *vm)
{}


/* > Store the binary-coded decimal equivalent of the value stored in register Vx at
 *   addresses I, I + 1, and I + 2. */
void 
CH8INSTR_Fx33(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;

    uint8_t bcd1 = (vm->cpu->Vx[x] % 10u);
    uint8_t bcd2 = (vm->cpu->Vx[x] % 100u - vm->cpu->Vx[x] % 10u) / 10u;
    uint8_t bcd3 = (vm->cpu->Vx[x] % 1000u - vm->cpu->Vx[x] % 100u) / 100u;

    vm->cpu->I = (bcd3 << 8u) | (bcd2 << 4u) | bcd1; // todo: fix Clang-Tidy
}


/* > Store the values of registers V0 to Vx inclusive in memory starting at address I.
 * > I is set to I + x + 1 after operation. */
void 
CH8INSTR_Fx55(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;

    for (unsigned i = 0; i < x; i++)
        vm->mem[vm->cpu->I + i] = vm->cpu->Vx[i];
}


/* > Fill registers V0 to Vx inclusive with the values stored in memory starting at
 *   address I.
 * > I is set to I + x + 1 after operation */
void 
CH8INSTR_Fx65(CH8_VM *vm)
{
    uint8_t x = (vm->current_opcode & 0x0F00u) >> 8u;

    for (unsigned i = 0; i < x; i++)
        vm->cpu->Vx[i] = vm->mem[vm->cpu->I + i];
}
