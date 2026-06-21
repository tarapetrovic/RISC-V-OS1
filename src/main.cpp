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

int main()
{
    MemoryAllocator::kinit();
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);
    // Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    void* p1 = mem_alloc(64);
    __putc(p1 ? 'Y' : 'N');
    void* p2 = mem_alloc(128);
    __putc(p2 ? 'Y' : 'N');
    int r = mem_free(p1);
    __putc(r == 0 ? 'Y' : 'N');
    r = mem_free(p2);
    __putc(r == 0 ? 'Y' : 'N');


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

