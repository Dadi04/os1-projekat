#include "../h/Scheduler.hpp"
#include "../h/TCB.hpp"
#include "../h/List.hpp"

static List<TCB> readyQueue;

void Scheduler::put(TCB *thread) {
    if (thread) readyQueue.addLast(thread);
}

TCB* Scheduler::get() {
    return readyQueue.removeFirst();
}
