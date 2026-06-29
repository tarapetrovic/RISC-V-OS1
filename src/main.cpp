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

volatile uint64 counterA = 0;
volatile uint64 counterB = 0;

void workerA(void*) {
    for (int i = 0; i < 5; i++) {
        counterA++;
        thread_dispatch();
    }
}

void workerB(void*) {
    for (int i = 0; i < 4; i++) {
        counterB++;
        thread_dispatch();
    }
}

int main() {

    MemoryAllocator::kinit();
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    TCB::running = TCB::createThread(nullptr, nullptr, nullptr);

    thread_t handleA, handleB;
    thread_create(&handleA, workerA, nullptr);
    thread_create(&handleB, workerB, nullptr);

    while (!((TCB*)handleA)->isFinished() ||
           !((TCB*)handleB)->isFinished()) {
        thread_dispatch();
    }

    __putc('A'); __putc(':'); __putc('0' + counterA); __putc('\n');
    __putc('B'); __putc(':'); __putc('0' + counterB); __putc('\n');
    __putc('D'); __putc('o'); __putc('n'); __putc('e'); __putc('\n');

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

