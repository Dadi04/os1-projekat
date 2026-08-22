#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/BoundedBuffer.hpp"
#include "../h/print.hpp"

extern void userMain();

extern BoundedBuffer* inputBuffer;
extern BoundedBuffer* outputBuffer;

void main() {
    MemoryAllocator::init();

    inputBuffer = new BoundedBuffer(256);
    outputBuffer = new BoundedBuffer(256);

    RiscV::w_stvec((uint64)&RiscV::supervisorTrap);

    RiscV::unmask_interrupts();

    TCB* mainThread = TCB::createThread(nullptr, nullptr, nullptr);
    TCB::running = mainThread;

    size_t stackBlocks = (DEFAULT_STACK_SIZE + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    uint64* idleStack = (uint64*)MemoryAllocator::alloc(stackBlocks);
    TCB::createThread([](void*) {
        while (true) {
            thread_dispatch();
        }
    }, nullptr, idleStack);

    thread_t userThread;
    thread_create(&userThread, [](void*) {
        userMain();
    }, nullptr);

    while (!((TCB*)userThread)->isFinished()) {
        thread_dispatch();
    }

    while (outputBuffer->getCnt() > 0) {
        thread_dispatch();
    }
}