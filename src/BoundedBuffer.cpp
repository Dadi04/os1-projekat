#include "../h/BoundedBuffer.hpp"
#include "../h/MemoryAllocator.hpp"

BoundedBuffer::BoundedBuffer(int capacity) : capacity(capacity), head(0), tail(0), count(0) {
    buffer = (char*)MemoryAllocator::alloc((capacity * sizeof(char) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE * MEM_BLOCK_SIZE);

    itemAvailable = new _sem(0);
}

BoundedBuffer::~BoundedBuffer() {
    MemoryAllocator::free(buffer);
    delete itemAvailable;
}

void BoundedBuffer::put(char val) {
    if (count < capacity) {
        buffer[tail] = val;
        tail = (tail + 1) % capacity;
        count++;
        itemAvailable->signal();
    }
}

char BoundedBuffer::get() {
    itemAvailable->wait();

    char ret = buffer[head];
    head = (head + 1) % capacity;
    count--;

    return ret;
}