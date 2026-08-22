#include "../h/TCB.hpp"
#include "../h/RiscV.hpp"
#include "../h/Scheduler.hpp"

TCB* TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB* TCB::createThread(Body body, void *arg, uint64* stackSpace) {
    return new TCB(body, arg, stackSpace, TIME_SLICE);
}

TCB* TCB::createKernelThread(Body body, void *arg, uint64* stackSpace) {
    TCB* t = new TCB(body, arg, stackSpace, TIME_SLICE);
    t->isKernel = true;
    return t;
}

void TCB::yield() {
    __asm__ volatile ("li a0, 0x13; ecall" : : : "a0");
}

void TCB::dispatch() {
    TCB* old = running;
    if (!old->isFinished()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();
    TCB::timeSliceCounter = 0;
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper() {
    if (running->isKernel) {
        RiscV::ms_sstatus(RiscV::SSTATUS_SIE);
    } else {
        RiscV::popSppSpie();
    }
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}

void TCB::block() {
    TCB* old = running;
    running = Scheduler::get();
    timeSliceCounter = 0;
    contextSwitch(&old->context, &running->context);
}

void TCB::unblock(TCB *thread) {
    if (thread) Scheduler::put(thread);
}