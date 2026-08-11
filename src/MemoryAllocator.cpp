#include "../h/MemoryAllocator.hpp"

MemoryAllocator::FreeMem* MemoryAllocator::head = nullptr;

void MemoryAllocator::init() {
    head = (FreeMem*)HEAP_START_ADDR;
    head->size = (size_t)((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR);
    head->next = nullptr;
}

void *MemoryAllocator::alloc(size_t blocks) {
    size_t requestedBytes = blocks * MEM_BLOCK_SIZE;

    size_t headerSize = ((sizeof(FreeMem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE;     
            // ((16B + 64 - 1) / 64) * 64 = (79B / 64) * 64 = 1B * 64 = 64B = 1 block
            // 16B struct, 48B padding
    size_t totalBytes = requestedBytes + headerSize;

    FreeMem* prev = nullptr;
    FreeMem* curr = head;

    while (curr != nullptr) {
        if (curr->size >= totalBytes) {
            if (curr->size >= totalBytes + headerSize + MEM_BLOCK_SIZE) {
                FreeMem* newFree = (FreeMem*)((char*)curr + totalBytes);
                newFree->size = curr->size - totalBytes;
                newFree->next = curr->next;
                curr->size = totalBytes;

                if (prev != nullptr) prev->next = newFree;
                else head = newFree;
            } else {
                if (prev != nullptr) prev->next = curr->next;
                else head = curr->next;
            }

            return (void*)((char*)curr + headerSize);
        }

        prev = curr;
        curr = curr->next;
    }

    return nullptr;
}

int MemoryAllocator::free(void *ptr) {
    if (ptr == nullptr) return -1;

    size_t headerSize = ((sizeof(FreeMem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE;     

    FreeMem* freed = (FreeMem*)((char*)ptr - headerSize);

    if ((char*)freed < (char*)HEAP_START_ADDR || (char*)freed >= (char*)HEAP_END_ADDR) return -1;

    FreeMem* prev = nullptr;
    FreeMem* curr = head;

    while (curr != nullptr && curr < freed) {
        prev = curr;
        curr = curr->next;
    }

    if (prev != nullptr) prev->next = freed;
    else head = freed;
    freed->next = curr;

    tryMerge(freed);
    if (prev != nullptr) tryMerge(prev);
    return 0;
}

void MemoryAllocator::tryMerge(FreeMem *curr) {
    if (curr == nullptr || curr->next == nullptr) return;

    if ((char*)curr + curr->size == (char*)curr->next) {
        curr->size += curr->next->size;
        curr->next = curr->next->next;
    }
}
