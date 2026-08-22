#include "../h/SleepingQueue.hpp"
#include "../h/Scheduler.hpp"

SleepingQueue::Elem* SleepingQueue::head = nullptr;

void SleepingQueue::put(TCB* thread, time_t time) {
    if (thread == nullptr || time == 0) return;

    Elem* prev = nullptr;
    Elem* curr = head;

    while (curr != nullptr && time >= curr->timeLeft) {
        time -= curr->timeLeft;
        prev = curr;
        curr = curr->next;
    }

    Elem* elem = new Elem(thread, time, curr);

    if (curr != nullptr) {
        curr->timeLeft -= time;
    }

    if (prev == nullptr) {
        head = elem;
    } else {
        prev->next = elem;
    }
} 

void SleepingQueue::update() {
    if (head == nullptr) return;

    head->timeLeft--;

    while (head != nullptr && head->timeLeft == 0) {
        Elem* old = head;
        head = head->next;
        Scheduler::put(old->thread);
        delete old;
    }
} 