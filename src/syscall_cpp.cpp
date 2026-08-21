#include "../h/syscall_cpp.hpp"

void* operator new(size_t size) {
    return mem_alloc(size);
}

void operator delete(void* ptr) {
    mem_free(ptr);
}

Thread::Thread(void (*body)(void*), void* arg) : myHandle(nullptr), body(body), arg(arg) {}

Thread::Thread() : myHandle(nullptr), body(nullptr), arg(nullptr) {}

Thread::~Thread() {}

int Thread::start() {
    return thread_create(&myHandle, &threadBody, this);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

void Thread::threadBody(void* arg) {
    Thread* t = (Thread*)arg;
    if (t->body != nullptr) {
        t->body(t->arg);
    } else {
        t->run();
    }
}

Semaphore::Semaphore(unsigned init) : myHandle(nullptr) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

PeriodicThread::PeriodicThread(time_t period) : Thread(periodicLoop, (void*)this), period(period) {}

void PeriodicThread::periodicLoop(void* arg) {
    PeriodicThread* pt = (PeriodicThread*)arg;
    while (pt->period > 0) {
        pt->periodicActivation();
        time_sleep(pt->period);
    }
}

void PeriodicThread::terminate() {
    period = 0;
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}