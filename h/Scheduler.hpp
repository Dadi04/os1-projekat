#ifndef _SCHEDULER_HPP_
#define _SCHEDULER_HPP_

class TCB;

class Scheduler {
public:
    static void put(TCB* thread);
    static TCB* get();
};

#endif