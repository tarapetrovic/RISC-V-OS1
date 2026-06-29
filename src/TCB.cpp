//
// Created by os on 6/20/26.
//
// Kod preuzet sa 7. vezbi

#include "../h/TCB.hpp"
#include "../h/Riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../lib/console.h"

TCB *TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(TCB::Body body, TCB::Argument arg, void *stack_space) {
    return new TCB(body, DEFAULT_TIME_SLICE, arg, stack_space); // proveri da li default time slice ili onaj zakomentarisani??
}


void TCB::yield() {
    thread_dispatch();
}

void TCB::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->arg); // Start the Thread body to execute, and pass the argument to body function
//    running->setFinished(true);
//    thread_dispatch(); // yield or just directly call thread_dispatch()?
    thread_exit();
}

void TCB::dispatch() {
    TCB *old = running;
    if (!old->isFinished()) Scheduler::put(old);

    running = Scheduler::get();
    contextSwitch(&old->context, &running->context);
}

int TCB::exit() {
    if (running == nullptr || running->isFinished()) return -1;
    running->setFinished(true);
    dispatch();
    return 0;
}