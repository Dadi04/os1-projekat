#include "../h/MemoryAllocator.hpp"

void *operator new(__SIZE_TYPE__ n) {
    uint64 blocks = (n + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return MemoryAllocator::alloc(blocks);
}

void *operator new[](__SIZE_TYPE__ n) {
    uint64 blocks = (n + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return MemoryAllocator::alloc(blocks);
}

void operator delete(void *p) noexcept {
    MemoryAllocator::free(p);
}

void operator delete[](void *p) noexcept {
    MemoryAllocator::free(p);
}