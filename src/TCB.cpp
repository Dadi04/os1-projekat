#include "../h/TCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/Scheduler.hpp"

TCB* TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB* TCB::createThread(Body body, void *arg) {
    return new TCB(body, arg, TIME_SLICE);
}

void TCB::yield() {
    __asm__ volatile ("ecall");
}

void TCB::dispatch() {
    TCB* old = TCB::running;
    if (!old->isFinished()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper() {
    RiscV::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}