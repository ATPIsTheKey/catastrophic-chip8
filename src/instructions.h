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

#ifndef CATASTROPHIC_CHIP8_INSTRUCTIONS_H
#define CATASTROPHIC_CHIP8_INSTRUCTIONS_H

#include "vm.h"

/*** Opcode implementations */

void CH8_INSTR_0nnn(CH8_VM *vm);

void CH8_INSTR_00E0(CH8_VM *vm);

void CH8_INSTR_00EE(CH8_VM *vm);

void CH8_INSTR_1nnn(CH8_VM *vm);

void CH8_INSTR_2nnn(CH8_VM *vm);

void CH8_INSTR_3xkk(CH8_VM *vm);

void CH8_INSTR_4xkk(CH8_VM *vm);

void CH8_INSTR_5xy0(CH8_VM *vm);

void CH8_INSTR_6xkk(CH8_VM *vm);

void CH8_INSTR_7xkk(CH8_VM *vm);

void CH8_INSTR_8xy0(CH8_VM *vm);

void CH8_INSTR_8xy1(CH8_VM *vm);

void CH8_INSTR_8xy2(CH8_VM *vm);

void CH8_INSTR_8xy3(CH8_VM *vm);

void CH8_INSTR_8xy4(CH8_VM *vm);

void CH8_INSTR_8xy5(CH8_VM *vm);

void CH8_INSTR_8xy6(CH8_VM *vm);

void CH8_INSTR_8xy7(CH8_VM *vm);

void CH8_INSTR_8xyE(CH8_VM *vm);

void CH8_INSTR_9xy0(CH8_VM *vm);

void CH8_INSTR_Annn(CH8_VM *vm);

void CH8_INSTR_Bnnn(CH8_VM *vm);

void CH8_INSTR_Cxkk(CH8_VM *vm);

void CH8_INSTR_Dxyn(CH8_VM *vm);

void CH8_INSTR_Ex9E(CH8_VM *vm);

void CH8_INSTR_ExA1(CH8_VM *vm);

void CH8_INSTR_Fx07(CH8_VM *vm);

void CH8_INSTR_Fx0A(CH8_VM *vm);

void CH8_INSTR_Fx15(CH8_VM *vm);

void CH8_INSTR_Fx18(CH8_VM *vm);

void CH8_INSTR_Fx1E(CH8_VM *vm);

void CH8_INSTR_Fx29(CH8_VM *vm);

void CH8_INSTR_Fx33(CH8_VM *vm);

void CH8_INSTR_Fx55(CH8_VM *vm);

void CH8_INSTR_Fx65(CH8_VM *vm);

/*** Opcode selector functions for opcodes of type 000_, 8xy_, Ex__, FX__.
 *** b represents variable part of opcode identifier */

int  CH8_INSTR_000b(CH8_VM *vm);

int  CH8_INSTR_8xyb(CH8_VM *vm);

int  CH8_INSTR_Exbb(CH8_VM *vm);

int  CH8_INSTR_Fxbb(CH8_VM *vm);

/*** Main function to execute an opcode */

int  CH8_INSTR_exec(CH8_VM *vm);

#endif //CATASTROPHIC_CHIP8_INSTRUCTIONS_H
