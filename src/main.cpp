//
// Created by os on 6/20/26.
//
// Kod preuzet sa 7. vezbi

#include "../h/Riscv.hpp"
// #include "../lib/console.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.hpp"
#include "../h/TCB.hpp"
// #include "../h/syscall_cpp.hpp"


extern void userMain(); // defined in userMain.cpp

// wrapper so we can launch userMain as a thread body (needs void(*)(void*) shape)
static void userMainWrapper(void*) {
    userMain();
}

int main() {

    MemoryAllocator::kinit();
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    TCB::running = TCB::createThread(nullptr, nullptr, nullptr); // main thread

    // create a thread over userMain
    thread_t userThread;
    thread_create(&userThread, userMainWrapper, nullptr);

    // let the userMain thread (and everything it spawns) run to completion
    while (!((TCB*)userThread)->isFinished()) {
        thread_dispatch();
    }

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

