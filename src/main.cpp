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

// ---- tiny print helpers (only main/initial thread prints, never workers) ----
static void printStr(const char* s) { while (*s) { __putc(*s); s++; } }
static void printNum(long n) {
    if (n < 0) { __putc('-'); n = -n; }
    if (n >= 10) printNum(n / 10);
    __putc('0' + (n % 10));
}
static void report(const char* label, long got, long expected) {
    printStr(label); printStr(": got "); printNum(got);
    printStr(", expected "); printNum(expected);
    printStr(got == expected ? "  [PASS]\n" : "  [FAIL]\n");
}

// helper: dispatch until all listed threads report finished
static void drain(thread_t* handles, int n) {
    bool allDone = false;
    while (!allDone) {
        allDone = true;
        for (int i = 0; i < n; i++)
            if (!((TCB*)handles[i])->isFinished()) allDone = false;
        if (!allDone) thread_dispatch();
    }
}

// ===========================================================================
// TEST 1: basic wait/signal handoff. sem=0, waiter blocks, signaler wakes it.
// ===========================================================================
static sem_t t1_sem;
static volatile int t1_signalDone = 0;
static volatile int t1_wokeAfterSignal = -1;

static void t1_waiter(void*) {
    sem_wait(t1_sem);
    t1_wokeAfterSignal = t1_signalDone;   // 1 iff signaler already ran
}
static void t1_signaler(void*) {
    t1_signalDone = 1;
    sem_signal(t1_sem);
}

// ===========================================================================
// TEST 2: mutex — 4 threads x 1000 increments under sem(1). Expect 4000.
// ===========================================================================
static sem_t t2_mutex;
static volatile long t2_counter = 0;
static void t2_worker(void*) {
    for (int i = 0; i < 1000; i++) {
        sem_wait(t2_mutex);
        long tmp = t2_counter; tmp++; t2_counter = tmp;
        sem_signal(t2_mutex);
        thread_dispatch();
    }
}

// ===========================================================================
// TEST 3: wait_n/signal_n mixed sizes + FIFO. Waiters need 5,1,3 (in order).
// ===========================================================================
static sem_t t3_sem;
static volatile int t3_order[3];
static volatile int t3_idx = 0;
static volatile int t3_w5_done = 0, t3_w1_done = 0, t3_w3_done = 0;
static void t3_w5(void*) { sem_wait_n(t3_sem, 5); t3_order[t3_idx++] = 5; t3_w5_done = 1; }
static void t3_w1(void*) { sem_wait_n(t3_sem, 1); t3_order[t3_idx++] = 1; t3_w1_done = 1; }
static void t3_w3(void*) { sem_wait_n(t3_sem, 3); t3_order[t3_idx++] = 3; t3_w3_done = 1; }

// ===========================================================================
// TEST 4: sem_close wakes blocked waiters with error (<0).
// ===========================================================================
static sem_t t4_sem;
static volatile int t4_ret_a = 999, t4_ret_b = 999;
static void t4_a(void*) { t4_ret_a = sem_wait(t4_sem); }
static void t4_b(void*) { t4_ret_b = sem_wait(t4_sem); }

// ===========================================================================
// TEST 5: wait_n immediate success when value suffices. sem=10.
// ===========================================================================
static sem_t t5_sem;
static volatile int t5_r1 = 999, t5_r2 = 999;
static void t5_taker(void*) {
    t5_r1 = sem_wait_n(t5_sem, 4);   // 10 -> 6
    t5_r2 = sem_wait_n(t5_sem, 6);   // 6 -> 0
}

int main() {

    MemoryAllocator::kinit();
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    TCB::running = TCB::createThread(nullptr, nullptr, nullptr);

    printStr("\n##### SEMAPHORE TESTS #####\n");

    // ---- TEST 1 ----
    {
        sem_open(&t1_sem, 0);
        thread_t b, a;
        thread_create(&b, t1_waiter, nullptr);
        thread_create(&a, t1_signaler, nullptr);
        thread_t hs[] = { b, a };
        drain(hs, 2);
        report("T1 waiter woke after signal", t1_wokeAfterSignal, 1);
        sem_close(t1_sem);
    }

    // ---- TEST 2 ----
    {
        sem_open(&t2_mutex, 1);
        t2_counter = 0;
        thread_t th[4];
        for (int i = 0; i < 4; i++) thread_create(&th[i], t2_worker, nullptr);
        drain(th, 4);
        report("T2 counter (mutex)", t2_counter, 4000);
        sem_close(t2_mutex);
    }

    // ---- TEST 3 ----
    {
        sem_open(&t3_sem, 0);
        t3_idx = 0; t3_w5_done = t3_w1_done = t3_w3_done = 0;
        thread_t w5, w1, w3;
        thread_create(&w5, t3_w5, nullptr);
        thread_create(&w1, t3_w1, nullptr);
        thread_create(&w3, t3_w3, nullptr);

        // let all three block on wait_n (sem is 0)
        thread_dispatch(); thread_dispatch(); thread_dispatch(); thread_dispatch();
        report("T3 stage0 nobody woke", t3_idx, 0);

        sem_signal_n(t3_sem, 4);                 // head needs 5, only 4 -> none
        thread_dispatch(); thread_dispatch();
        report("T3 stage1 still nobody", t3_idx, 0);

        sem_signal_n(t3_sem, 1);                 // now 5 -> w5 wakes, value 0
        thread_dispatch(); thread_dispatch();
        report("T3 stage2 w5 done", t3_w5_done, 1);
        report("T3 stage2 idx==1", t3_idx, 1);

        sem_signal_n(t3_sem, 4);                 // 4 -> w1(needs1) wakes ->3 -> w3(needs3) wakes ->0
        thread_t rest[] = { w1, w3, w5 };
        drain(rest, 3);
        report("T3 order[0]", t3_order[0], 5);
        report("T3 order[1]", t3_order[1], 1);
        report("T3 order[2]", t3_order[2], 3);
        sem_close(t3_sem);
    }

    // ---- TEST 4 ----
    {
        sem_open(&t4_sem, 0);
        t4_ret_a = t4_ret_b = 999;
        thread_t a, b;
        thread_create(&a, t4_a, nullptr);
        thread_create(&b, t4_b, nullptr);
        thread_dispatch(); thread_dispatch(); thread_dispatch();  // both block

        sem_close(t4_sem);                        // wake both with error
        thread_t hs[] = { a, b };
        drain(hs, 2);
        report("T4 a error<0", (t4_ret_a < 0) ? 1 : 0, 1);
        report("T4 b error<0", (t4_ret_b < 0) ? 1 : 0, 1);
        // sem already closed+deleted — do NOT touch t4_sem again
    }

    // ---- TEST 5 ----
    {
        sem_open(&t5_sem, 10);
        thread_t t;
        thread_create(&t, t5_taker, nullptr);
        thread_t hs[] = { t };
        drain(hs, 1);
        report("T5 wait_n(4) ok", t5_r1, 0);
        report("T5 wait_n(6) ok", t5_r2, 0);
        sem_close(t5_sem);
    }

    printStr("##### TESTS COMPLETE #####\n");
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

