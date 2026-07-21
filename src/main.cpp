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
#include "../h/syscall_cpp.hpp"

// ---- print helpers (only main prints, between tests) ----
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

// dispatch until a shared "done count" reaches the expected number
static void drainUntil(volatile int* doneCounter, int expected) {
    while (*doneCounter < expected) thread_dispatch();
}

// ===========================================================================
// TEST 1: global new / delete (operator new -> mem_alloc)
// Allocate an array via new[], write to it, free via delete[].
// If new routes through mem_alloc correctly, this works without crashing.
// ===========================================================================
static void testNewDelete() {
    int* arr = new int[16];
    for (int i = 0; i < 16; i++) arr[i] = i * i;
    long sum = 0;
    for (int i = 0; i < 16; i++) sum += arr[i];
    delete[] arr;
    // sum of i^2 for i=0..15 = 1240
    report("T1 new/delete array sum", sum, 1240);

    // single-object new/delete
    int* p = new int(42);
    long v = *p;
    delete p;
    report("T1 new/delete single", v, 42);
}

// ===========================================================================
// TEST 2: Thread via FUNCTION-POINTER constructor.
// A plain function increments a counter. Confirms body path + start().
// ===========================================================================
static volatile int t2_counter = 0;
static volatile int t2_done = 0;
static void t2_body(void*) {
    for (int i = 0; i < 5; i++) { t2_counter++; thread_dispatch(); }
    t2_done = 1;
}
static void testThreadFnPointer() {
    Thread t(t2_body, nullptr);
    t.start();
    drainUntil(&t2_done, 1);
    report("T2 fn-pointer thread ran", t2_counter, 5);
}

// ===========================================================================
// TEST 3: Thread via SUBCLASS overriding run().
// Confirms the default-ctor -> wrapper -> run() dispatch path.
// ===========================================================================
static volatile int t3_counter = 0;
static volatile int t3_done = 0;
class T3Thread : public Thread {
public:
    T3Thread() : Thread() {}
protected:
    void run() override {
        for (int i = 0; i < 7; i++) { t3_counter++; thread_dispatch(); }
        t3_done = 1;
    }
};
static void testThreadSubclass() {
    T3Thread t;
    t.start();
    drainUntil(&t3_done, 1);
    report("T3 subclass run() ran", t3_counter, 7);
}

// ===========================================================================
// TEST 4: body WINS over run() (spec precedence rule).
// A subclass overrides run() BUT is constructed with the function-pointer
// ctor. Per spec, run() must be IGNORED and the function pointer used.
// We set two different flags: the fn-pointer sets t4_body_ran, run() would
// set t4_run_ran. Only the body flag must end up set.
// ===========================================================================
static volatile int t4_body_ran = 0;
static volatile int t4_run_ran = 0;
static volatile int t4_done = 0;
static void t4_body(void*) { t4_body_ran = 1; t4_done = 1; }
class T4Thread : public Thread {
public:
    // constructed WITH a function pointer, even though run() is overridden
    T4Thread() : Thread(t4_body, nullptr) {}
protected:
    void run() override { t4_run_ran = 1; t4_done = 1; }
};
static void testBodyWinsOverRun() {
    T4Thread t;
    t.start();
    drainUntil(&t4_done, 1);
    report("T4 body ran", t4_body_ran, 1);
    report("T4 run() did NOT run", t4_run_ran, 0);
}

// ===========================================================================
// TEST 5: Semaphore (C++ wrapper) — mutex over a shared counter.
// Two function-pointer threads increment under a Semaphore(1).
// ===========================================================================
static Semaphore* t5_mutex = nullptr;   // pointer so we control its lifetime
static volatile long t5_counter = 0;
static volatile int t5_done = 0;
static void t5_worker(void*) {
    for (int i = 0; i < 500; i++) {
        t5_mutex->wait();
        long tmp = t5_counter; tmp++; t5_counter = tmp;
        t5_mutex->signal();
        thread_dispatch();
    }
    t5_done++;
}
static void testSemaphoreCpp() {
    t5_mutex = new Semaphore(1);   // uses global new -> mem_alloc; ctor -> sem_open
    t5_counter = 0; t5_done = 0;
    Thread a(t5_worker, nullptr);
    Thread b(t5_worker, nullptr);
    a.start();
    b.start();
    drainUntil(&t5_done, 2);
    report("T5 sem mutex counter", t5_counter, 1000);
    delete t5_mutex;               // dtor -> sem_close; global delete -> mem_free
    t5_mutex = nullptr;
}

// ===========================================================================
// TEST 6: Semaphore blocking handoff (C++ wrapper).
// sem starts at 0. Waiter blocks on wait(); signaler signals(). Confirms the
// C++ wrapper's wait/signal forward correctly and blocking works.
// ===========================================================================
static Semaphore* t6_sem = nullptr;
static volatile int t6_signalDone = 0;
static volatile int t6_wokeAfterSignal = -1;
static volatile int t6_done = 0;
static void t6_waiter(void*) {
    t6_sem->wait();
    t6_wokeAfterSignal = t6_signalDone;
    t6_done++;
}
static void t6_signaler(void*) {
    t6_signalDone = 1;
    t6_sem->signal();
    t6_done++;
}
static void testSemaphoreBlocking() {
    t6_sem = new Semaphore(0);
    t6_signalDone = 0; t6_wokeAfterSignal = -1; t6_done = 0;
    Thread w(t6_waiter, nullptr);
    Thread s(t6_signaler, nullptr);
    w.start();
    s.start();
    drainUntil(&t6_done, 2);
    report("T6 waiter woke after signal", t6_wokeAfterSignal, 1);
    delete t6_sem;
    t6_sem = nullptr;
}

int main() {

    MemoryAllocator::kinit();
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    TCB::running = TCB::createThread(nullptr, nullptr, nullptr);

    printStr("\n##### C++ API TESTS #####\n");
    testNewDelete();
    testThreadFnPointer();
    testThreadSubclass();
    testBodyWinsOverRun();
    testSemaphoreCpp();
    testSemaphoreBlocking();
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

