#include "../h/syscall_c.h"

void* mem_alloc(size_t size) {
    size_t blocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    void* ret;

    __asm__ volatile (
        "mv a1, %[blk]\n"
        "li a0, 0x01\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [blk] "r"(blocks)
        : "a0", "a1"
    );

    return ret;
}

int mem_free(void *ptr) {
    int ret;

    __asm__ volatile (
        "mv a1, %[ptr]\n"
        "li a0, 0x02\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [ptr] "r"(ptr)
        : "a0", "a1"
    );

    return ret;
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg) {
    void* stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    if (stack_space == nullptr) return -1;
    
    int ret;

    __asm__ volatile (
        "mv a4, %[stk]\n"
        "mv a3, %[arg]\n"
        "mv a2, %[body]\n"
        "mv a1, %[hnd]\n"
        "li a0, 0x11\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [hnd] "r"(handle), [body] "r"(start_routine), [arg] "r"(arg), [stk] "r"(stack_space)
        : "a0", "a1", "a2", "a3", "a4"
    );

    return ret;
}

int thread_exit() {
    int ret;

    __asm__ volatile (
        "li a0, 0x12\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : 
        : "a0"
    );

    return ret;
}

void thread_dispatch() {
    __asm__ volatile (
        "li a0, 0x13\n"
        "ecall\n"
        : 
        : 
        : "a0"
    );
}

char getc() {
    int ret;

    __asm__ volatile (
        "li a0, 0x41\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : 
        : "a0"
    );

    return (char)ret;
}

void putc(char c) {
    __asm__ volatile (
        "mv a1, %[c]\n"
        "li a0, 0x42\n"
        "ecall\n"
        : 
        : [c] "r"(c)
        : "a0", "a1"
    );
}