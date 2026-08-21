#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/_sem.hpp"
#include "../h/SleepingQueue.hpp"
#include "../h/BoundedBuffer.hpp"
#include "../h/print.hpp"

BoundedBuffer* inputBuffer = nullptr;
BoundedBuffer* outputBuffer = nullptr;

void RiscV::popSppSpie() {
    RiscV::mc_sstatus(RiscV::SSTATUS_SPP);
    RiscV::ms_sstatus(RiscV::SSTATUS_SPIE);
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void RiscV::handleSupervisorTrap(uint64 *savedRegs) {
    uint64 scause = r_scause();
    if (scause == ECALL_USER || scause == ECALL_SUPERVISOR) {
        // interrupt no, cause code: environment call from U-mode and S-mode
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        uint64 code = savedRegs[A0];
        uint64 a1 = savedRegs[A1];
        uint64 a2 = savedRegs[A2];
        uint64 a3 = savedRegs[A3];
        uint64 a4 = savedRegs[A4];

        switch (code) {
            case MEM_ALLOC:
                savedRegs[A0] = (uint64)MemoryAllocator::alloc((size_t)a1);
                break;
            case MEM_FREE:
                savedRegs[A0] = (uint64)MemoryAllocator::free((void*)a1);
                break;
            case THREAD_CREATE: {
                TCB** handle = (TCB**)a1;
                TCB::Body body = (TCB::Body)a2;
                void* arg = (void*)a3;
                uint64* stackSpace = (uint64*)a4;

                TCB* t = TCB::createThread(body, arg, stackSpace);
                if (t != nullptr) {
                    *handle = t;
                    savedRegs[A0] = 0;
                } else {
                    savedRegs[A0] = (uint64)-1;
                }
                break;
            }
            case THREAD_EXIT:
                TCB::running->setFinished(true);
                TCB::dispatch();
                break;
            case THREAD_DISPATCH:
                TCB::timeSliceCounter = 0;
                TCB::dispatch();
                break;
            case SEM_OPEN: {
                _sem** handle = (_sem**)a1;
                unsigned init = (unsigned)a2;
                *handle = new _sem(init);
                savedRegs[A0] = (*handle != nullptr) ? 0 : (uint64)-1;
                break;
            }
            case SEM_CLOSE: {
                _sem* sem = (_sem*)a1;
                savedRegs[A0] = sem->close();
                delete sem;
                break;
            }
            case SEM_WAIT: {
                _sem* sem = (_sem*)a1;
                savedRegs[A0] = sem->wait();
                break;
            }
            case SEM_SIGNAL: {
                _sem* sem = (_sem*)a1;
                savedRegs[A0] = sem->signal();
                break;
            }
            case SEM_WAIT_N: {
                _sem* sem = (_sem*)a1;
                unsigned n = (unsigned)a2;
                savedRegs[A0] = sem->waitN(n);
                break;
            }
            case SEM_SIGNAL_N: {
                _sem* sem = (_sem*)a1;
                unsigned n = (unsigned)a2;
                savedRegs[A0] = sem->signalN(n);
                break;
            }
            case TIME_SLEEP: {
                time_t time = (time_t)a1;
                if (time > 0) {
                    SleepingQueue::put(TCB::running, time);
                    TCB::block();
                    savedRegs[A0] = 0;
                }
                break;
            }
            case GETC: {
                savedRegs[A0] = (uint64)inputBuffer->get();
                break;
            }
            case PUTC: {
                char c = (char)a1;
                while (!(*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT)) {}
                *((char*)CONSOLE_TX_DATA) = c;
                savedRegs[A0] = 0;
                break;
            }
            default:
                break;
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    } else if (scause == TIMER_INTERRUPT) {
        // interrupt: yes, cause code: supervisor software interrupt (timer)
        mc_sip(SIP_SSIP);
        SleepingQueue::update();
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
    } else if (scause == CONSOLE_INTERRUPT) {
        // interrupt: yes, cause code: supervisor external interrupt (console)
        int irq = plic_claim();
        if (irq == CONSOLE_IRQ) {
            while (*((char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT) {
                char c = *((char*)CONSOLE_RX_DATA);
                inputBuffer->put(c);
            }
        }
        plic_complete(irq);
    } else {
        printString("User exception! SCAUSE: ");
        printInt(scause);
        printString("\n");
        while (true) {}
    }
}