#ifndef _PRINT_HPP_
#define _PRINT_HPP_

#include "syscall_c.h"

#ifdef __cplusplus
extern "C" {
#endif

void printString(const char *str);

void printInt(long long num, int base = 10);

void printHex(unsigned long long num);

#ifdef __cplusplus
}
#endif

#endif