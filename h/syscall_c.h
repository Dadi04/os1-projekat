#ifndef _SYSCALL_C_H_
#define _SYSCALL_C_H_

#include "../lib/hw.h"

#ifdef __cplusplus
extern "C" {
#endif

void* mem_alloc(size_t size);
int mem_free(void*);

typedef void* thread_t;
int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);
int thread_exit();
void thread_dispatch();

char getc();
void putc(char c);

#ifdef __cplusplus
}
#endif

#endif