//
// Created by os on 6/24/26.
//

#include "../h/Scheduler.hpp"
#include "../h/TCB.hpp"

List<TCB> Scheduler::readyThreadQueue;
SleepingList Scheduler::sleepingThreadQueue;

TCB *Scheduler::get()
{
    return readyThreadQueue.removeFirst();
}

void Scheduler::put(TCB *ccb)
{
    readyThreadQueue.addLast(ccb);
}

int Scheduler::putToSleep(time_t time) {
    if (time == 0) { return 0; } // dont do anything, not really sleeping
    TCB* running = TCB::running;
    if (running == nullptr || running->isFinished()) { return -1; }

    running->setBlocked(true); // sleeping and blocking are sharing a variable, think about dividing them to blocked and sleeping, or if this is ok
    sleepingThreadQueue.insert(running, time);
    TCB::dispatch(); // kernel-internal dispatch, thread_dispatch would be a syscall from kernel, unnecessary
    return 0;
}

void Scheduler::updateSleeping() {
    sleepingThreadQueue.tick();
    TCB* woken;
    while ((woken = sleepingThreadQueue.removeExpired()) != nullptr) {
        woken->setBlocked(false);
        put(woken); // wake up and put in scheduler
    }
}