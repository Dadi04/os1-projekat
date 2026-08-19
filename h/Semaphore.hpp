#ifndef _SEMAPHORE_HPP_
#define _SEMAPHORE_HPP_

#include "List.hpp"

class TCB;

class Semaphore {
public:
    Semaphore(unsigned init) : val(init), closed(false) {}
    ~Semaphore() { close(); }

    int wait();
    int signal();

    int waitN(unsigned n);
    int signalN(unsigned n);

    int close();
private:
    int val;
    bool closed;
    List<TCB> blockedQueue;

    void block();
    void unblock();
};

#endif