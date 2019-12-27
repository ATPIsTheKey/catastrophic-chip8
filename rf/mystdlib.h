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

#ifndef CATASTROPHIC_CHIP8_MYSTDLIB_H
#define CATASTROPHIC_CHIP8_MYSTDLIB_H

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#ifdef DEBUG
#define DBGPRINT(fmt, args ...) fprintf(stdout, KRED" %s:%d:%s(): "KNRM fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
#define DBGPRINT(fmt, args...)
#endif

#define NP_CHECK(ptr)                                                          \
    {                                                                          \
        if ((ptr) == NULL) {                                                     \
            fprintf(stderr, "%s:%d NULL POINTER: %s n",                        \
                __FILE__, __LINE__, #ptr);                                     \
            exit(-1);                                                          \
        }                                                                      \
    }                                                                          \
//

#endif //CATASTROPHIC_CHIP8_MYSTDLIB_H
