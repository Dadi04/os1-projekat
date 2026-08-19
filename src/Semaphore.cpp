#include "../h/Semaphore.hpp"
#include "../h/TCB.hpp"
#include "../h/Scheduler.hpp"

int Semaphore::wait() {
    if (closed) return -1;

    val--;
    if (val < 0) {
        block();
    }
    
    if (closed) return -1;
    return 0;
}

int Semaphore::signal() {
    if (closed) return -1;

    val++;
    if (val <= 0) {
        unblock();
    }
    return 0;
}

int Semaphore::waitN(unsigned n) {
    if (closed) return -1;

    val -= (int)n;
    while (val < 0) {
        block();
        if (closed) return -1;
    }

    return 0;
}

int Semaphore::signalN(unsigned n) {
    if (closed) return -1;

    for (unsigned i = 0; i < n; i++) {
        val++;
        if (val <= 0) {
            unblock();
        }
    }

    return 0;
}

int Semaphore::close() {
    if (closed) return -1;
    closed = true;

    while (!blockedQueue.isEmpty()) {
        unblock();
    }
    return 0;
}

void Semaphore::block() {
    blockedQueue.addLast(TCB::running);
    TCB::block();
}

void Semaphore::unblock() {
    TCB* t = blockedQueue.removeFirst();
    TCB::unblock(t);
}