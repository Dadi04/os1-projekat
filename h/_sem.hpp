#ifndef __SEM_HPP_
#define __SEM_HPP_

#include "List.hpp"
#include "MemoryAllocator.hpp"

class TCB;

class _sem {
public:
    _sem(unsigned init) : val(init), closed(false) {}
    ~_sem() { close(); }

    int wait();
    int signal();

    int waitN(unsigned n);
    int signalN(unsigned n);

    int close();

    void* operator new(size_t size) { return MemoryAllocator::alloc(size); }
    void operator delete(void* ptr) { MemoryAllocator::free(ptr); }
private:
    int val;
    bool closed;
    List<TCB> blockedQueue;

    void block();
    void unblock();
};

#endif