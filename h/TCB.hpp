#ifndef _TCB_HPP_
#define _TCB_HPP_

#include "../lib/hw.h"
#include "Scheduler.hpp"

class TCB {
public:
    ~TCB() { delete[] stack; }

    using Body = void (*)(void*);
    static TCB* createThread(Body body, void *arg);

    static void yield();

    bool isFinished() const { return finished; };
    void setFinished(bool finished) { TCB::finished = finished; };

    uint64 getTimeSlice() const { return timeSlice; };
    void setTimeSlice(uint64 timeSlice) { TCB::timeSlice = timeSlice; }

    static TCB *running;
private:
    TCB(Body body, void *arg, uint64 timeSlice) : 
            body(body), 
            arg(arg),
            stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr), 
            context({
                (uint64)&threadWrapper,
                stack != nullptr ? (uint64)&stack[STACK_SIZE] : 0
            }), 
            timeSlice(timeSlice),
            finished(false) {
        if (body != nullptr) Scheduler::put(this);
    }
    struct Context {
        uint64 ra;
        uint64 sp;
    };
    
    Body body;
    void *arg;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;

    friend class RiscV;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *newContext);

    static void dispatch();

    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = DEFAULT_STACK_SIZE / sizeof(uint64);
    static uint64 constexpr TIME_SLICE = 2;
};

#endif