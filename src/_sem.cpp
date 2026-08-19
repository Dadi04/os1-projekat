#include "../h/_sem.hpp"
#include "../h/TCB.hpp"
#include "../h/Scheduler.hpp"

int _sem::wait() {
    if (closed) return -1;

    val--;
    if (val < 0) {
        block();
    }
    
    if (closed) return -1;
    return 0;
}

int _sem::signal() {
    if (closed) return -1;

    val++;
    if (val <= 0) {
        unblock();
    }
    return 0;
}

int _sem::waitN(unsigned n) {
    if (closed) return -1;

    val -= (int)n;
    while (val < 0) {
        block();
        if (closed) return -1;
    }

    return 0;
}

int _sem::signalN(unsigned n) {
    if (closed) return -1;

    for (unsigned i = 0; i < n; i++) {
        val++;
        if (val <= 0) {
            unblock();
        }
    }

    return 0;
}

int _sem::close() {
    if (closed) return -1;
    closed = true;

    while (!blockedQueue.isEmpty()) {
        unblock();
    }
    return 0;
}

void _sem::block() {
    blockedQueue.addLast(TCB::running);
    TCB::block();
}

void _sem::unblock() {
    TCB* t = blockedQueue.removeFirst();
    TCB::unblock(t);
}