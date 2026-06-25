//
// Created by os on 6/20/26.
//
// Kod preuzet sa 7. vezbi

// #include "../h/tcb.hpp"
// #include "../h/workers.hpp"
// #include "../h/print.hpp"
#include "../h/Riscv.hpp"
#include "../lib/console.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.hpp"
#include "../h/TCB.hpp"

void workerA(void* arg) {
    for(int i = 0; i < 3; i++) __putc('A');
}

void workerB(void* arg) {
    for(int i = 0; i < 3; i++) __putc('B');
}

void workerC(void* arg) {
    for(int i = 0; i < 3; i++) __putc('C');
}

int main()
{
    MemoryAllocator::kinit();
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);
    TCB::running = TCB::createThread(nullptr, nullptr, nullptr);

    void* stackA = mem_alloc(DEFAULT_STACK_SIZE);
    void* stackB = mem_alloc(DEFAULT_STACK_SIZE);
    void* stackC = mem_alloc(DEFAULT_STACK_SIZE);

    TCB* tcbA = TCB::createThread(workerA, nullptr, stackA);
    TCB* tcbB = TCB::createThread(workerB, nullptr, stackB);
    TCB* tcbC = TCB::createThread(workerC, nullptr, stackC);

    while (!tcbA->isFinished() || !tcbB->isFinished() || !tcbC->isFinished()) {
        TCB::yield();
    }

    __putc('D');
    __putc('o');
    __putc('n');
    __putc('e');
    return 0;


    /* Riscv::w_stvec((uint64)&Riscv::supervisorTrap);
    //Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
    __asm__ volatile("ecall");
    __putc('O'); __putc('K');
*/
    /*
    TCB *threads[5];

    threads[0] = TCB::createThread(nullptr);
    TCB::running = threads[0];

    threads[1] = TCB::createThread(workerBodyA);
    printString("ThreadA created\n");
    threads[2] = TCB::createThread(workerBodyB);
    printString("ThreadB created\n");
    threads[3] = TCB::createThread(workerBodyC);
    printString("ThreadC created\n");
    threads[4] = TCB::createThread(workerBodyD);
    printString("ThreadD created\n");

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while (!(threads[1]->isFinished() &&
             threads[2]->isFinished() &&
             threads[3]->isFinished() &&
             threads[4]->isFinished()))
    {
        TCB::yield();
    }

    for (auto &thread: threads)
    {
        delete thread;
    }
    printString("Finished\n");
*/
    return 0;
}

