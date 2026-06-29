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

void workerA(void* arg) {
    for(int i = 0; i < 5; i++) {
        counterA++;
    }
}

void workerB(void* arg) {
    for(int i = 0; i < 5; i++) {
        counterB++;
    }
}
int main() {
    // initialize allocator
    MemoryAllocator::kinit();
    __putc('1');

    // initialize trap vector
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);
    __putc('2');

    // create main thread (privileged, body=nullptr)
    TCB* threads[3];
    threads[0] = TCB::createThread(nullptr, nullptr, nullptr);
    TCB::running = threads[0];
    __putc('3');

    // allocate stacks and create user threads
    void* stackA = MemoryAllocator::kmem_alloc(
            (DEFAULT_STACK_SIZE + sizeof(size_t) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE
    );
    void* stackB = MemoryAllocator::kmem_alloc(
            (DEFAULT_STACK_SIZE + sizeof(size_t) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE
    );
    __putc('4');

    threads[1] = TCB::createThread(workerA, nullptr, (char*)stackA + DEFAULT_STACK_SIZE);
    __putc('5');
    threads[2] = TCB::createThread(workerB, nullptr, (char*)stackB + DEFAULT_STACK_SIZE);
    __putc('6');

    // run until all threads done
    while (!threads[1]->isFinished() || !threads[2]->isFinished()) {
        __putc('p');
        TCB::yield();
    }
    __putc(counterA);
    __putc(counterB);

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

