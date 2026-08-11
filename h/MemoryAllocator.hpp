#ifndef _MEMORY_ALLOCATOR_HPP_
#define _MEMORY_ALLOCATOR_HPP_

#include "../lib/hw.h"

class MemoryAllocator {
public:
    static void init();
    static void* alloc(size_t blocks);      // size in blocks
    static int free(void* ptr);             // 0 success, <0 error
private:
    struct FreeMem {
        size_t size;                        // size in bytes
        FreeMem* next;
    };

    static FreeMem* head;
    
    static void tryMerge(FreeMem* curr);
};

#endif