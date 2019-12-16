//
// Created by roland on 2019-12-16.
//

#ifndef CATASTROPHIC_CHIP8_STDUTILS_H
#define CATASTROPHIC_CHIP8_STDUTILS_H

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#ifdef DEBUG
#define DBGPRINT(fmt, args ...) fprintf(stdout, KMAG" %s:%d:%s(): "KNRM fmt, \
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

#endif //CATASTROPHIC_CHIP8_STDUTILS_H
