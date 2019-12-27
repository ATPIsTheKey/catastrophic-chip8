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

#include "debug.h"

#include <stdio.h>

#include "../rf/mystdlib.h"


int
CH8_VM_DBG_log(const char *callee, const char *fmt, ...)
{
    int rc = fprintf(stderr, KRED"%s(): "KNRM"%s", callee, fmt);
    return rc;
}


int
CH8_VM_DBG_output_cpu_dump(const char *callee, CH8_VM *vm, const char *fmt, ...)
{
    fprintf(stderr, KRED"%s(): "KNRM"%s", callee, fmt);
    fprintf(stderr,"    op: 0x%04X pc: 0x%03X sp: 0x%02X I: 0x%03X\n",
            vm->current_opcode, vm->cpu->pc, vm->cpu->sp, vm->cpu->I);

    fprintf(stderr, "    registers: ");
    for (uint8_t i=0; i < 16; i++) {
        fprintf(stderr,"0x%02X ", vm->cpu->V[i]);
    }

    fprintf(stderr,"\n");
    fprintf(stderr,"    stack:    ");
    if (vm->cpu->sp == 0) {
        fprintf(stderr,"empty");
    } else {
        for (uint8_t i=0; i < 16 && i < vm->cpu->sp; i++) {
            fprintf(stderr,"0x%03X ", vm->cpu->stack[i]);
        }
    }
    fprintf(stderr,"\n");
    return 0;
}
