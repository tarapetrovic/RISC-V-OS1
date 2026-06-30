//
// Created by os on 6/29/26.
//

#include "../h/_sem.hpp"


int Semaphore::wait_n(unsigned n) {
    if (closed) return -1;

    if (value >= n) {
        value -= n;
        return 0;
    }
    block(n);

    if (closed) return -1; // ovde se vracaju niti koje se odblokiraju, ako je semafor zatvoren dok su bile blokirane javi gresku
    return 0;
}

int Semaphore::signal_n(unsigned n) {
    if (closed) return -1;

    value += n;

    while (blockedThreads.peekFirst() && (unsigned)value >= blockedThreads.peekFirst()->getWaitingCount()) {
        unblock();
    }

    return 0;
}

int Semaphore::close() {
    if (closed) return -1;
    closed = true;

    while (blockedThreads.peekFirst()) {
        TCB* head = blockedThreads.removeFirst();
        head->setBlocked(false);
        Scheduler::put(head);
    }
}

void Semaphore::block(unsigned n) {
    TCB* running = TCB::running;
    running->waitingCount = n;
    running->setBlocked(true);
    blockedThreads.addLast(TCB::running);
    TCB::dispatch(); // ili thread_dispatch()????
}

void Semaphore::unblock() {
    TCB* head = blockedThreads.removeFirst();
    value -= head->getWaitingCount();
    head->setBlocked(false);
    Scheduler::put(head);
}