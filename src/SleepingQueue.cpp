#include "../h/SleepingQueue.hpp"
#include "../h/Scheduler.hpp"

SleepingQueue::Elem* SleepingQueue::head = nullptr;
SleepingQueue::Elem* SleepingQueue::tail = nullptr;

void SleepingQueue::put(TCB* thread, time_t time) {
    if (thread == nullptr || time == 0) return;

    thread->setSleepTime(time);

    Elem* elem = new Elem(thread);
    if (head == nullptr) {
        head = tail = elem;
    } else {
        tail->next = elem;
        tail = elem;
    }
} 

void SleepingQueue::update() {
    Elem* curr = head;
    Elem* prev = nullptr;

    while (curr != nullptr) {
        curr->thread->decSleepTime();

        if (curr->thread->getSleepTime() == 0) {
            Scheduler::put(curr->thread);

            Elem* old = curr;
            if (prev == nullptr) {
                head = curr->next;
            } else {
                prev->next = curr->next;
            }

            if (old == tail) {
                tail = prev;
            }
            curr = curr->next;
            delete old;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
} 