#include "../h/RiscV.hpp"
#include "../h/TCB.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Semaphore.hpp"

void RiscV::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void RiscV::handleSupervisorTrap(uint64 *savedRegs) {
    uint64 scause = r_scause();
    if (scause == ECALL_USER || scause == ECALL_SUPERVISOR) {
        // interrupt no, cause code: environment call from U-mode and S-mode
        uint64 sepc = r_sepc() + 4;
        uint64 sstatus = r_sstatus();

        uint64 code = savedRegs[A0];     // a0
        uint64 a1 = savedRegs[A1];       // a1
        uint64 a2 = savedRegs[A2];       // a2
        uint64 a3 = savedRegs[A3];       // a3
        uint64 a4 = savedRegs[A4];       // a4

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
                Semaphore** handle = (Semaphore**)a1;
                unsigned init = (unsigned)a2;
                *handle = new Semaphore(init);
                savedRegs[A0] = (*handle != nullptr) ? 0 : (uint64)-1;
                break;
            }
            case SEM_CLOSE: {
                Semaphore* sem = (Semaphore*)a1;
                savedRegs[A0] = sem->close();
                delete sem;
                break;
            }
            case SEM_WAIT: {
                Semaphore* sem = (Semaphore*)a1;
                savedRegs[A0] = sem->wait();
                break;
            }
            case SEM_SIGNAL: {
                Semaphore* sem = (Semaphore*)a1;
                savedRegs[A0] = sem->signal();
                break;
            }
            case SEM_WAIT_N: {
                Semaphore* sem = (Semaphore*)a1;
                unsigned n = (unsigned)a2;
                savedRegs[A0] = sem->waitN(n);
                break;
            }
            case SEM_SIGNAL_N: {
                Semaphore* sem = (Semaphore*)a1;
                unsigned n = (unsigned)a2;
                savedRegs[A0] = sem->signalN(n);
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
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
            uint64 sepc = r_sepc();
            uint64 sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
    } else if (scause == CONSOLE_INTERRUPT) {
        // interrupt: yes, cause code: supervisor external interrupt (console)
    } else {
        // unexpected trap cause
    }
}