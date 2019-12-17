//
// Created by roland on 2019-12-17.
//

#include <string.h>

#include "instructions.h"

/* > Execute machine language subroutine at address NNN. */
void 
CHIP8INSTR_0NNN(CHIP8_VM *vm)
{}


/* > Clear the screen. */
void 
CHIP8INSTR_00E0(CHIP8_VM *vm)
{
    memset(vm->framebuffer, 0x00, sizeof(vm->framebuffer));
}


/* > Return from a subroutine. */
void 
CHIP8INSTR_00EE(CHIP8_VM *vm)
{
    vm->cpu->pc = vm->cpu->stack[vm->cpu->sp];
    vm->cpu->sp--;
}


/* > Jump to address NNN. */
void 
CHIP8INSTR_1NNN(CHIP8_VM *vm)
{
    uint16_t NNN = (vm->current_opcode & 0x0FFFu);

    vm->cpu->pc = NNN;
}


/* > Execute subroutine starting at address NNN. */
void 
CHIP8INSTR_2NNN(CHIP8_VM *vm)
{
    uint16_t NNN = (vm->current_opcode & 0x0FFFu);

    vm->cpu->sp++;
    vm->cpu->stack[vm->cpu->sp] = vm->cpu->pc;
    vm->cpu->pc = NNN;
}


/* > Skip the following instruction if the value of register VX equals KK. */
void 
CHIP8INSTR_3XKK(CHIP8_VM *vm)
{
    uint8_t X  = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t KK = (vm->current_opcode & 0x00FFu);

    if (vm->cpu->VX[X] == KK)
        vm->cpu->pc += 2;
}


/* > Skip the following instruction if the value of register VX is not equal to KK. */
void 
CHIP8INSTR_4XKK(CHIP8_VM *vm)
{
    uint8_t X  = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t KK = (vm->current_opcode & 0x00FFu);

    if (vm->cpu->VX[X] != KK)
        vm->cpu->pc += 2;
}


/* > Skip the following instruction if the value of register VX is equal to the value
 *   of register VY. */
void 
CHIP8INSTR_5XY0(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t Y = (vm->current_opcode & 0x0F00u) >> 4u;

    if (vm->cpu->VX[X] == vm->cpu->VX[Y])
        vm->cpu->pc += 2;
}


/* > Store number KK in register VX. */
void 
CHIP8INSTR_6XKK(CHIP8_VM *vm)
{
    uint8_t X  = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t KK = (vm->current_opcode & 0x00FFu);

    vm->cpu->VX[X] = KK;
}


/* > Add the value KK to register VX. */
void 
CHIP8INSTR_7XKK(CHIP8_VM *vm)
{
    uint8_t X  = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t KK = (vm->current_opcode & 0x00FFu);

    vm->cpu->VX[X] += KK;
}


/* > Store the value of register VY in register VX. */
void 
CHIP8INSTR_8XY0(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t Y = (vm->current_opcode & 0x00F0u) >> 4u;

    vm->cpu->VX[X] = vm->cpu->VX[Y];
}


/* > Set VX to VX OR VY. */
void 
CHIP8INSTR_8XY1(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t Y = (vm->current_opcode & 0x00F0u) >> 4u;

    vm->cpu->VX[X] |= vm->cpu->VX[Y];
}


/* > Set VX to VX AND VY. */
void 
CHIP8INSTR_8XY2(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t Y = (vm->current_opcode & 0x00F0u) >> 4u;

    vm->cpu->VX[X] &= vm->cpu->VX[Y];
}


/* > Set VX to VX XOR VY. */
void 
CHIP8INSTR_8XY3(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t Y = (vm->current_opcode & 0x00F0u) >> 4u;

    vm->cpu->VX[X] ^= vm->cpu->VX[Y];
}


/* > Add the value of register VY to register VX.
 * > Set VF to 01 if a carry occurs.
 * > Set VF to 00 if a carry does not occur. */
void 
CHIP8INSTR_8XY4(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t Y = (vm->current_opcode & 0x00F0u) >> 4u;

    uint16_t r = vm->cpu->VX[X] + vm->cpu->VX[Y];
    if (r > 255u) {
        vm->cpu->VX[0x0F] = 1u;
    } else {
        vm->cpu->VX[0x0F] = 0u;
    }

    vm->cpu->VX[X] = r & 0x00FF;
}


/* > Subtract the value of register VY from register VX
 * > Set VF to 00 if a borrow occurs
 * > Set VF to 01 if a borrow does not occur */
void 
CHIP8INSTR_8XY5(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t Y = (vm->current_opcode & 0x00F0u) >> 4u;

    if (vm->cpu->VX[X] > vm->cpu->VX[Y]) {
        vm->cpu->VX[0x0F] = 1u;
    } else {
        vm->cpu->VX[0x0F] = 0u;
    }

    vm->cpu->VX[X] -= vm->cpu->VX[Y];
}


/* > Store the value of register VY shifted right one bit in register VX.
 * > Set register VF to the least significant bit prior to the shift.
 * > VY is unchanged. */
void 
CHIP8INSTR_8XY6(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;

    vm->cpu->VX[0x0F] = (vm->cpu->VX[X] & 0x01u);
    vm->cpu->VX[X] >>= 1u;
}


/* > Set register VX to the value of VY minus VX.
 * > Set VF to 00 if a borrow occurs.
 * > Set VF to 01 if a borrow does not occur. */
void 
CHIP8INSTR_8XY7(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t Y = (vm->current_opcode & 0x00F0u) >> 4u;

    if (vm->cpu->VX[Y] > vm->cpu->VX[X]) {
        vm->cpu->VX[0x0F] = 1u;
    } else {
        vm->cpu->VX[0x0F] = 0u;
    }

    vm->cpu->VX[X] = vm->cpu->VX[Y] - vm->cpu->VX[X];
}


/* > Shifts the value of register VX to the left one bit.
 * > Set register VF to the most significant bit prior to the shift.
 * > VY is unchanged. */
void 
CHIP8INSTR_8XYE(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;

    vm->cpu->VX[0x0F] = (vm->cpu->VX[X] & 0x80u) >> 7u;
    vm->cpu->VX[X] <<= 1u;
}


/* > Skip the following instruction if the value of register VX is not equal to the
 * > value of register VY. */
void 
CHIP8INSTR_9XY0(CHIP8_VM *vm)
{
    uint8_t X = (vm->current_opcode & 0x0F00u) >> 8u;
    uint8_t Y = (vm->current_opcode & 0x00F0u) >> 4u;

    if (vm->cpu->VX[X] != vm->cpu->VX[Y])
        vm->cpu->pc += 2;
}


/* > Store memory address NNN in register I. */
void 
CHIP8INSTR_ANNN(CHIP8_VM *vm)
{}


/* > Jump to address NNN + V0. */
void 
CHIP8INSTR_BNNN(CHIP8_VM *vm)
{}


/* > Set VX to a random number with a mask of KK. */
void 
CHIP8INSTR_CXKK(CHIP8_VM *vm)
{}


/* > Draw a sprite at position VX, VY with N bytes of sprite data starting at the
 *   address stored in I.
 * > Set VF to 01 if any set pixels are changed to unset, and 00 otherwise. */
void 
CHIP8INSTR_DXYN(CHIP8_VM *vm)
{}


/* > Skip the following instruction if the key corresponding to the hex value 
 *   currently stored in register VX is pressed */
void 
CHIP8INSTR_EX9E(CHIP8_VM *vm)
{}


/* > Skip the following instruction if the key corresponding to the hex value 
 *   currently stored in register VX is not pressed*/
void 
CHIP8INSTR_EXA1(CHIP8_VM *vm)
{}


/* > Store the current value of the delay timer in register VX. */
void 
CHIP8INSTR_FX07(CHIP8_VM *vm)
{}


/* > Wait for a keypress and store the result in register VX. */
void 
CHIP8INSTR_FX0A(CHIP8_VM *vm)
{}


/* > Set the delay timer to the value of register VX. */
void 
CHIP8INSTR_FX15(CHIP8_VM *vm)
{}


/* > Set the sound timer to the value of register VX. */
void 
CHIP8INSTR_FX18(CHIP8_VM *vm)
{}


/* > Add the value stored in register VX to register I. */
void 
CHIP8INSTR_FX1E(CHIP8_VM *vm)
{}


/* > Set I to the memory address of the sprite data corresponding to the hexadecimal
 *   digit stored in register VX. */
void 
CHIP8INSTR_FX29(CHIP8_VM *vm)
{}


/* > Store the binary-coded decimal equivalent of the value stored in register VX at
 *   addresses I, I + 1, and I + 2. */
void 
CHIP8INSTR_FX33(CHIP8_VM *vm)
{}


/* > Store the values of registers V0 to VX inclusive in memory starting at address I.
 * > I is set to I + X + 1 after operation. */
void 
CHIP8INSTR_FX55(CHIP8_VM *vm)
{}


/* > Fill registers V0 to VX inclusive with the values stored in memory starting at
 *   address I.
 * > I is set to I + X + 1 after operation */
void 
CHIP8INSTR_FX65(CHIP8_VM *vm)
{}
