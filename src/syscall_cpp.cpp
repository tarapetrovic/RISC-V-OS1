//
// Created by os on 7/20/26.
//

#include "../h/syscall_cpp.hpp"

// --- Thread ---

Thread::Thread(void (*body)(void *), void *arg) {
    myHandle = nullptr;
    this->body = body;
    this->arg = arg;
}

Thread::Thread() {
    myHandle = nullptr;
    body = &wrapper;
    arg = this;
}

Thread::~Thread() noexcept {
    // ?????? TODO
}

int Thread::start() {
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

void Thread::wrapper(void* thr) {
    ((Thread*)thr)->run();
}

// --- Semaphore ---

Semaphore::Semaphore(unsigned init) {
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

// --- Periodic Thread ---
// TODO asinhrona

// --- Console ---
// TODO na kraju
char Console::getc() { return ::getc(); } // call global getc from c api, thats why ::
void Console::putc(char c) { ::putc(c); } // call global putc from c api, thats why ::