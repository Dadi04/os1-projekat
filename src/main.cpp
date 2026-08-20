#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/BoundedBuffer.hpp"
#include "../h/syscall_c.h"

extern void userMain();

extern BoundedBuffer* inputBuffer;
extern BoundedBuffer* outputBuffer;

void main() {
    MemoryAllocator::init();

    inputBuffer = new BoundedBuffer(256);
    outputBuffer = new BoundedBuffer(256);

    RiscV::w_stvec((uint64)&RiscV::supervisorTrap);
    
    RiscV::ms_sie(1 << 9);
    
    RiscV::ms_sstatus(RiscV::SSTATUS_SIE);

    TCB* mainThread = TCB::createThread(nullptr, nullptr, nullptr);
    TCB::running = mainThread;

    thread_t userThread;
    thread_create(&userThread, [](void*) {
        userMain();
    }, nullptr);

    while (!((TCB*)userThread)->isFinished()) {
        thread_dispatch();
    }
}