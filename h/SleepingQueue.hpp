#ifndef _SLEEPING_QUEUE_HPP_
#define _SLEEPING_QUEUE_HPP_

#include "TCB.hpp"
#include "MemoryAllocator.hpp"

class SleepingQueue {
public:
    static void put(TCB* thread, time_t time);
    static void update();
private:
    struct Elem {
        TCB* thread;
        Elem* next;
        Elem(TCB* thread, Elem* next = nullptr) : thread(thread), next(next) {}

        void* operator new(size_t size) { return MemoryAllocator::alloc(size); }
        void operator delete(void* ptr) { MemoryAllocator::free(ptr); }
    };

    static Elem* head;
    static Elem* tail;
};

#endif