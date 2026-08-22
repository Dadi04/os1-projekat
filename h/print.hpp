#ifndef _PRINT_HPP_
#define _PRINT_HPP_

#include "syscall_c.h"

#ifdef __cplusplus
extern "C" {
#endif

void kputc(char c);

void kprintString(const char *str);

void kprintInt(uint64 num);

#ifdef __cplusplus
}
#endif

#endif