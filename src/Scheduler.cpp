#include "../h/Scheduler.hpp"

struct SchedulerNode {
    TCB* thread;
    SchedulerNode* next;
    
    SchedulerNode(TCB* t) : thread(t), next(nullptr) {}
};

static SchedulerNode* head = nullptr;
static SchedulerNode* tail = nullptr;

void Scheduler::put(TCB *thread) {
    if (thread == nullptr) return;

    SchedulerNode* node = new SchedulerNode(thread);

    if (tail != nullptr) {
        tail->next = node;
    } else {
        head = node;
    }
    tail = node;
}

TCB* Scheduler::get() {
    if (head == nullptr) return nullptr;

    SchedulerNode* node = head;
    TCB* thread = node->thread;

    head = head->next;
    if (head == nullptr) tail = nullptr;

    delete node;
    return thread;
}
