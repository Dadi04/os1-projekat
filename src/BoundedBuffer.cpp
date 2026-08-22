#include "../h/BoundedBuffer.hpp"
#include "../h/MemoryAllocator.hpp"

BoundedBuffer::BoundedBuffer(int capacity) : capacity(capacity), head(0), tail(0), count(0) {
    buffer = (char*)MemoryAllocator::alloc((capacity * sizeof(char) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE * MEM_BLOCK_SIZE);

    itemAvailable = new _sem(0);
    spaceAvailable = new _sem(capacity);
}

BoundedBuffer::~BoundedBuffer() {
    MemoryAllocator::free(buffer);
    delete itemAvailable;
    delete spaceAvailable;
}

void BoundedBuffer::put(char val) {
    spaceAvailable->wait();

    buffer[tail] = val;
    tail = (tail + 1) % capacity;
    count++;

    itemAvailable->signal();
}

bool BoundedBuffer::putFromISR(char in) {
    if (count >= capacity) {
        return false;
    }

    buffer[tail] = in;
    tail = (tail + 1) % capacity;
    count++;

    itemAvailable->signal();

    return true;
}

char BoundedBuffer::get() {
    itemAvailable->wait();

    char ret = buffer[head];
    head = (head + 1) % capacity;
    count--;

    spaceAvailable->signal();

    return ret;
}

bool BoundedBuffer::getFromISR(char* out) {
    if (count <= 0 || out == nullptr) {
        return false;
    } 

    *out = buffer[head];
    head = (head + 1) % capacity;
    count--;

    spaceAvailable->signal();
    
    return true;
}
