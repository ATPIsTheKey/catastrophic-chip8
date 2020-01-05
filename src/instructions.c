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

#include "instructions.h"

#include <stdlib.h>
#include <string.h>

#include "debug.h"

#define HIGH_PIXEL 0xFFFFFFFF

/*** Some naïve macros to hopefully increase code readability *****************/


#define X(opcode)   ( ((opcode) & 0x0F00u) >> 8u )
#define Y(opcode)   ( ((opcode) & 0x00F0u) >> 4u )
#define N(opcode)   ( (opcode) & 0x000Fu )
#define KK(opcode)  ( (opcode) & 0x00FFu )
#define NNN(opcode) ( (opcode) & 0x0FFFu )

#define CPU(vm_ptr) ((vm_ptr)->cpu) // Using this macro its just more clear
                                    // that we are manipulating the CPU of our
                                    // vm. Accessing all those nested sub-objects
                                    // with arrow operators is quite annoying to read.

/*** Implementation of CHIP8 opcodes ******************************************/


//> Execute machine language subroutine at address nnn.
void 
CH8_INSTR_0nnn(CH8_VM *vm) // instruction ignored by most chip8 emulators
{}


//> Clear the screen.
void 
CH8_INSTR_00E0(CH8_VM *vm)
{
    memset(vm->framebuffer, 0x00, sizeof(vm->framebuffer));
    vm->internal_flags |= CH8_VM_SCREEN_UPDATE;
}


//> Return from a subroutine.
void 
CH8_INSTR_00EE(CH8_VM *vm)
{
    CPU(vm)->sp--;
    CPU(vm)->pc = CPU(vm)->stack[CPU(vm)->sp];
}


//> Jump to address nnn.
void 
CH8_INSTR_1nnn(CH8_VM *vm)
{
    uint16_t nnn = NNN(vm->current_opcode);

    CPU(vm)->pc = nnn - 2; // we don't want to increment our stack pointer when
                           // jumping to an address
}


//> Execute subroutine starting at address nnn.
void
CH8_INSTR_2nnn(CH8_VM *vm)
{
    uint16_t nnn = (vm->current_opcode & 0x0FFFu);

    CPU(vm)->stack[CPU(vm)->sp] = CPU(vm)->pc;
    CPU(vm)->sp++;
    CPU(vm)->pc = nnn - 2; // we don't want to increment our stack pointer when
                           // jumping to a subroutine
}


//> Skip the following instruction if the value of register Vx equals kk.
void 
CH8_INSTR_3xkk(CH8_VM *vm)
{
    uint8_t x  = X(vm->current_opcode);
    uint8_t kk = KK(vm->current_opcode);

    if (CPU(vm)->V[x] == kk)
        CPU(vm)->pc += 2;
}


//> Skip the following instruction if the value of register Vx is not equal to kk.
void 
CH8_INSTR_4xkk(CH8_VM *vm)
{
    uint8_t x  = X(vm->current_opcode);
    uint8_t kk = KK(vm->current_opcode);

    if (CPU(vm)->V[x] != kk)
        CPU(vm)->pc += 2;
}


//> Skip the following instruction if the value of register Vx is equal to the value
//  of register VY.
void 
CH8_INSTR_5xy0(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    uint8_t y = Y(vm->current_opcode);

    if (CPU(vm)->V[x] == CPU(vm)->V[y])
        CPU(vm)->pc += 2;
}


//> Store number kk in register Vx.
void
CH8_INSTR_6xkk(CH8_VM *vm)
{
    uint8_t x  = X(vm->current_opcode);
    uint8_t kk = KK(vm->current_opcode);

    CPU(vm)->V[x] = kk;
}


//> Add the value kk to register Vx.
void 
CH8_INSTR_7xkk(CH8_VM *vm)
{
    uint8_t x  = X(vm->current_opcode);
    uint8_t kk = KK(vm->current_opcode);

    CPU(vm)->V[x] += kk;
}


//> Store the value of register Vy in register Vx.
void 
CH8_INSTR_8xy0(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    uint8_t y = Y(vm->current_opcode);

    CPU(vm)->V[x] = CPU(vm)->V[y];
}


//> Set Vx to Vx OR Vy.
void 
CH8_INSTR_8xy1(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    uint8_t y = Y(vm->current_opcode);

    CPU(vm)->V[x] |= CPU(vm)->V[y];
}


//> Set Vx to Vx AND Vy.
void 
CH8_INSTR_8xy2(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    uint8_t y = Y(vm->current_opcode);

    CPU(vm)->V[x] &= CPU(vm)->V[y];
}


//> Set Vx to Vx xOR Vy.
void 
CH8_INSTR_8xy3(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    uint8_t y = Y(vm->current_opcode);

    CPU(vm)->V[x] ^= CPU(vm)->V[y];
}


//> Add the value of register Vy to register Vx.
//> Set VF to 01 if a carry occurs.
//> Set VF to 00 if a carry does not occur.
void 
CH8_INSTR_8xy4(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    uint8_t y = Y(vm->current_opcode);

    uint16_t r = CPU(vm)->V[x] + CPU(vm)->V[y];
    if (r > 0xFFu)
        CPU(vm)->V[0xF] = 0x01u;
    else
        CPU(vm)->V[0xF] = 0x00u;

    CPU(vm)->V[x] = r & 0x00FFu;
}


//> Subtract the value of register Vy from register Vx
//> Set VF to 00 if a borrow occurs
//> Set VF to 01 if a borrow does not occur.
void 
CH8_INSTR_8xy5(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    uint8_t y = Y(vm->current_opcode);

    if (CPU(vm)->V[x] >= CPU(vm)->V[y])
        CPU(vm)->V[0xF] = 0x01u;
    else
        CPU(vm)->V[0xF] = 0x00u;

    CPU(vm)->V[x] -= CPU(vm)->V[y];
}


//> Store the value of register Vy shifted right one bit in register Vx.
//> Set register VF to the least significant bit prior to the shift.
//> Vy is unchanged.
void 
CH8_INSTR_8xy6(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    CPU(vm)->V[0xF] = (CPU(vm)->V[x] & 0x01u);
    CPU(vm)->V[x] >>= 0x01u;
}


//> Set register Vx to the value of Vy minus Vx.
//> Set VF to 00 if a borrow occurs.
//> Set VF to 01 if a borrow does not occur.
void 
CH8_INSTR_8xy7(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    uint8_t y = Y(vm->current_opcode);

    if (CPU(vm)->V[y] >= CPU(vm)->V[x])
        CPU(vm)->V[0xF] = 0x01u;
    else
        CPU(vm)->V[0xF] = 0x00u;

    CPU(vm)->V[x] = CPU(vm)->V[y] - CPU(vm)->V[x];
}


//> Shifts the value of register Vx to the left one bit.
//> Set register VF to the most significant bit prior to the shift.
//> Vy is unchanged.
void 
CH8_INSTR_8xyE(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    CPU(vm)->V[0xF] = (CPU(vm)->V[x] & 0x80u) >> 7u;
    CPU(vm)->V[x] <<= 1u;
}


//> Skip the following instruction if the value of register Vx is not equal to the
//> value of register Vy.
void 
CH8_INSTR_9xy0(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    uint8_t y = Y(vm->current_opcode);

    if (CPU(vm)->V[x] != CPU(vm)->V[y])
        CPU(vm)->pc += 2;
}


//> Store memory address nnn in register I.
void
CH8_INSTR_Annn(CH8_VM *vm)
{
    uint16_t nnn = NNN(vm->current_opcode);

    CPU(vm)->I = nnn;
}


//> Jump to address nnn + V0.
void 
CH8_INSTR_Bnnn(CH8_VM *vm)
{
    uint16_t nnn = NNN(vm->current_opcode);

    CPU(vm)->pc = nnn + CPU(vm)->V[0];
}


//> Set Vx to a random number with a mask of kk.
void
CH8_INSTR_Cxkk(CH8_VM *vm)
{
    uint8_t x  = X(vm->current_opcode);
    uint8_t kk = KK(vm->current_opcode);

    CPU(vm)->V[x] = (uint8_t)(rand() % 0xFFu + 1u) & kk;
}


//> Draw a sprite at position Vx, Vy with n bytes of sprite data starting at the
//  address stored in I.
//> Set VF to 01 if any set pixels are changed to unset, and 00 otherwise.
void 
CH8_INSTR_Dxyn(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    uint8_t y = Y(vm->current_opcode);
    uint8_t n = N(vm->current_opcode);

    uint8_t x_coord = CPU(vm)->V[x];
    uint8_t y_coord = CPU(vm)->V[y];

    CPU(vm)->V[0xF] = 0x00u;
    vm->internal_flags |= CH8_VM_SCREEN_UPDATE; // we are changing the framebuffer, so it has to be redrawn

    uint8_t sprite_byte;
    for (unsigned short y_line = 0; y_line < n; y_line++)
    {
        sprite_byte = vm->mem[CPU(vm)->I + y_line];

        for (unsigned short x_line = 0; x_line < 8; x_line++)
        {
            if ((sprite_byte & (0x80 >> x_line))) // get each individual bit
            {
                uint16_t pixel = ((x_coord + x_line) + ((y_coord + y_line) << 6)) % 2048;
                if (vm->framebuffer[pixel] == HIGH_PIXEL)
                    CPU(vm)->V[0xF] = 0x01u;
                vm->framebuffer[pixel] ^= HIGH_PIXEL;
            }
        }
    }
}


//> Skip the following instruction if the key corresponding to the hex value
//  currently stored in register Vx is pressed.
void 
CH8_INSTR_Ex9E(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    if (vm->keypad[CPU(vm)->V[x]])
        CPU(vm)->pc += 2;
}


//> Skip the following instruction if the key corresponding to the hex value
//  currently stored in register Vx is not pressed.
void 
CH8_INSTR_ExA1(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);
    
    if (!vm->keypad[CPU(vm)->V[x]])
        CPU(vm)->pc += 2;
}


//> Store the current value of the delay timer in register Vx.
void 
CH8_INSTR_Fx07(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    CPU(vm)->V[x] = CPU(vm)->delay_timer;
}


//> Wait for a keypress and store the result in register Vx.
void 
CH8_INSTR_Fx0A(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    int any_key_pressed = 0;
    for (uint16_t i = 0; i < (uint16_t) sizeof(vm->keypad); i++)
        if (vm->keypad[i]) {
            CPU(vm)->V[x] = i;
            any_key_pressed = 1;
            break;
        }

    if (!any_key_pressed)
        CPU(vm)->pc -= 2;
}


//> Set the delay timer to the value of register Vx.
void 
CH8_INSTR_Fx15(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    CPU(vm)->delay_timer = CPU(vm)->V[x];
}


//> Set the sound timer to the value of register Vx.
void 
CH8_INSTR_Fx18(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    CPU(vm)->sound_timer = CPU(vm)->V[x];
}


//> Add the value stored in register Vx to register I.
//> VF is set to 1 when ther is a range overflow (I+Vx > 0xFFF), and 0 when there
//  isn't.
void 
CH8_INSTR_Fx1E(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    if (CPU(vm)->I + CPU(vm)->V[x] > 0x0FFF)
        CPU(vm)->V[0xF] = 1u;
    else
        CPU(vm)->V[0xF] = 0u;

    CPU(vm)->I += CPU(vm)->V[x];
}


//> Set I to the memory address of the sprite data corresponding to the hexadecimal
//  digit stored in register Vx.
void 
CH8_INSTR_Fx29(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    CPU(vm)->I = CH8_VM_FONTSET_START_ADDR + CPU(vm)->V[x] * 5;
}


//> Store the binary-coded decimal equivalent of the value stored in register Vx at
//  addresses I, I + 1, and I + 2.
void 
CH8_INSTR_Fx33(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    uint8_t bcd100 =  CPU(vm)->V[x] / 100;
    uint8_t bcd10  = (CPU(vm)->V[x] / 10 ) % 10;
    uint8_t bcd1   =  CPU(vm)->V[x] % 10;

    vm->mem[CPU(vm)->I]     = bcd100;
    vm->mem[CPU(vm)->I + 1] = bcd10;
    vm->mem[CPU(vm)->I + 2] = bcd1;
}


//> Store the values of registers V0 to Vx inclusive in memory starting at address I.
//> I is set to I + x + 1 after operation.
void 
CH8_INSTR_Fx55(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    for (short i = 0; i <= x; i++)
        vm->mem[CPU(vm)->I + i] = CPU(vm)->V[i];

    if (vm->opt_flags & CH8_VM_ORIGINAL_IMPL) // Cowgod's Technical reference apparently
        CPU(vm)->I += x + 1;                  // describes opcodes 8xy6, 8xye, Fx55,
                                              // and Fx65 differently than other sources.
                                              // Since many roms found on the internet
                                              // are designed after Cowgod's reference,
                                              // one can choose which which instruction
                                              // implementations to use by setting the
                                              // CH8_VM_ORIGINAL_IMPL flag as an option.
}


//> Fill registers V0 to Vx inclusive with the values stored in memory starting at
//  address I.
//> I is set to I + x + 1 after operation
void 
CH8_INSTR_Fx65(CH8_VM *vm)
{
    uint8_t x = X(vm->current_opcode);

    for (short i = 0; i <= x; i++)
        CPU(vm)->V[i] = vm->mem[CPU(vm)->I + i];

    if (vm->opt_flags & CH8_VM_ORIGINAL_IMPL) // Cowgod's Technical reference apparently
        CPU(vm)->I += x + 1;                  // describes opcodes 8xy6, 8xye, Fx55,
                                              // and Fx65 differently than other sources.
                                              // Since many roms found on the internet
                                              // are designed after Cowgod's reference,
                                              // one can choose which which instruction
                                              // implementations to use by setting the
                                              // CH8_VM_ORIGINAL_IMPL flag as an option.
}


//> Execute chip8 instruction of type 0 with ending b
int
CH8_INSTR_000b(CH8_VM *vm)
{
    if ((vm->current_opcode & 0x0FFFu) != 0x00E0 && (vm->current_opcode & 0x0FFFu) != 0x00EE) {
        CH8_INSTR_0nnn(vm);
        return CH8_VM_SUCCESS;
    }

    switch (vm->current_opcode & 0x000Fu) {
        case 0x0000:
            CH8_INSTR_00E0(vm);
            break;

        case 0x000E:
            CH8_INSTR_00EE(vm);
            break;

        default:
           CH8_VM_DBG_log(__func__,
                   "Unsupported opcode: %x. Terminate execution.\n",
                   vm->current_opcode);
            return CH8_VM_UNSUPPORTED_OPCODE;
    }
    return CH8_VM_SUCCESS;
}


//> Execute chip8 instruction of type 8 with ending b
int
CH8_INSTR_8xyb(CH8_VM *vm)
{
    switch (vm->current_opcode & 0x000Fu) {
        case 0x0000:
            CH8_INSTR_8xy0(vm);
            break;

        case 0x0001:
            CH8_INSTR_8xy1(vm);
            break;

        case 0x0002:
            CH8_INSTR_8xy2(vm);
            break;

        case 0x0003:
            CH8_INSTR_8xy3(vm);
            break;

        case 0x0004:
            CH8_INSTR_8xy4(vm);
            break;

        case 0x0005:
            CH8_INSTR_8xy5(vm);
            break;

        case 0x0006:
            CH8_INSTR_8xy6(vm);
            break;

        case 0x0007:
            CH8_INSTR_8xy7(vm);
            break;

        case 0x000E:
            CH8_INSTR_8xyE(vm);
            break;

        default:
            CH8_VM_DBG_log(__func__,
                    "Unsupported opcode: %x. Terminate execution.\n",
                    vm->current_opcode);
            return CH8_VM_UNSUPPORTED_OPCODE;
    }
    return CH8_VM_SUCCESS;
}


//> Execute chip8 instruction of type E with ending bb
int
CH8_INSTR_Exbb(CH8_VM *vm)
{
    switch (vm->current_opcode & 0x00FFu) {
        case 0x009E:
            CH8_INSTR_Ex9E(vm);
            break;

        case 0x00A1:
            CH8_INSTR_ExA1(vm);
            break;

        default:
            CH8_VM_DBG_log(__func__,
                    "Unsupported opcode: %x. Terminate execution.\n",
                    vm->current_opcode);
            return CH8_VM_UNSUPPORTED_OPCODE;
    }
    return CH8_VM_SUCCESS;
}


//> Execute chip8 instruction of type F with ending bb
int
CH8_INSTR_Fxbb(CH8_VM *vm)
{
    switch (vm->current_opcode & 0x00FFu) {
        case 0x0007:
            CH8_INSTR_Fx07(vm);
            break;

        case 0x000A:
            CH8_INSTR_Fx0A(vm);
            break;

        case 0x0015:
            CH8_INSTR_Fx15(vm);
            break;

        case 0x0018:
            CH8_INSTR_Fx18(vm);
            break;

        case 0x001E:
            CH8_INSTR_Fx1E(vm);
            break;

        case 0x0029:
            CH8_INSTR_Fx29(vm);
            break;

        case 0x0033:
            CH8_INSTR_Fx33(vm);
            break;

        case 0x0055:
            CH8_INSTR_Fx55(vm);
            break;

        case 0x0065:
            CH8_INSTR_Fx65(vm);
            break;

        default:
            CH8_VM_DBG_log(__func__,
                    "Unsupported opcode: %x. Terminate execution.\n",
                    vm->current_opcode);
            return CH8_VM_UNSUPPORTED_OPCODE;
    }
    return CH8_VM_SUCCESS;
}


//> execute current opcode stored in vm
int
CH8_INSTR_exec(CH8_VM *vm)
{
    // instructions of type b can throw error codes as they also select instructions
    // based on the opcode
    switch (vm->current_opcode & 0xF000) {
        case 0x0000:
            if (CH8_INSTR_000b(vm) == CH8_VM_UNSUPPORTED_OPCODE)
                return CH8_VM_UNSUPPORTED_OPCODE;
            break;

        case 0x1000:
            CH8_INSTR_1nnn(vm);
            break;

        case 0x2000:
            CH8_INSTR_2nnn(vm);
            break;

        case 0x3000:
            CH8_INSTR_3xkk(vm);
            break;

        case 0x4000:
            CH8_INSTR_4xkk(vm);
            break;

        case 0x5000:
            CH8_INSTR_5xy0(vm);
            break;

        case 0x6000:
            CH8_INSTR_6xkk(vm);
            break;

        case 0x7000:
            CH8_INSTR_7xkk(vm);
            break;

        case 0x8000:
            if (CH8_INSTR_8xyb(vm) == CH8_VM_UNSUPPORTED_OPCODE)
                return CH8_VM_UNSUPPORTED_OPCODE;
            break;

        case 0x9000:
            CH8_INSTR_9xy0(vm);
            break;

        case 0xA000:
            CH8_INSTR_Annn(vm);
            break;

        case 0xB000:
            CH8_INSTR_Bnnn(vm);
            break;

        case 0xC000:
            CH8_INSTR_Cxkk(vm);
            break;

        case 0xD000:
            CH8_INSTR_Dxyn(vm);
            break;

        case 0xE000:
            if (CH8_INSTR_Exbb(vm) == CH8_VM_UNSUPPORTED_OPCODE)
                return CH8_VM_UNSUPPORTED_OPCODE;
            break;

        case 0xF000:
            if (CH8_INSTR_Fxbb(vm) == CH8_VM_UNSUPPORTED_OPCODE)
                return CH8_VM_UNSUPPORTED_OPCODE;
            break;

        default:
            CH8_VM_DBG_log(__func__,
                           "Unsupported opcode: %x. Terminate execution.\n",
                           vm->current_opcode
            );
            return CH8_VM_UNSUPPORTED_OPCODE;
    }
    return CH8_VM_SUCCESS;
}
