#ifndef _BOUNDED_BUFFER_HPP_
#define _BOUNDED_BUFFER_HPP_

#include "_sem.hpp"

class BoundedBuffer {
public:
    BoundedBuffer(int capacity);
    ~BoundedBuffer();

    void put(char val);
    char get();

    bool putFromISR(char val);
    bool getFromISR(char* val);

    int getCnt() const { return count; }

    void* operator new(size_t size) { return MemoryAllocator::alloc(size); }
    void operator delete(void* ptr) { MemoryAllocator::free(ptr); }
private:
    int capacity;
    char* buffer;
    int head, tail, count;

    _sem* itemAvailable;
    _sem* spaceAvailable;
};

#endif