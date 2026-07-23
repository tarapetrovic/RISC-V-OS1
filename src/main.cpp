//
// Created by os on 6/20/26.
//
// Kod preuzet sa 7. vezbi

#include "../h/Riscv.hpp"
#include "../lib/console.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.hpp"
#include "../h/TCB.hpp"
// #include "../h/syscall_cpp.hpp"

//
////// ------------ MAIN FOR TESTING OFFICIAL TESTS (20 POINTS) ------------------
//extern void userMain(); // defined in userMain.cpp
//
// // wrapper so we can launch userMain as a thread body (needs void(*)(void*) shape)
//static void userMainWrapper(void*) {
//    userMain();
//}

static void printStr(const char* s) { while (*s) { __putc(*s); s++; } }
static void printNum(uint64 n) {
    if (n >= 10) printNum(n / 10);
    __putc('0' + (n % 10));
}

static const uint64 WORK = 50000000;

static volatile uint64 counterA = 0, counterB = 0;
static volatile uint64 bSeenByA = 0;      // B's progress, sampled mid-run by A
static volatile bool doneA = false, doneB = false;

// NOTE: neither worker ever calls thread_dispatch()
static void spinA(void*) {
    for (uint64 i = 0; i < WORK; i++) {
        counterA++;
        if (i == WORK / 2) bSeenByA = counterB;   // snapshot at halfway
    }
    doneA = true;
}

static void spinB(void*) {
    for (uint64 i = 0; i < WORK; i++) { counterB++; }
    doneB = true;
}
extern volatile uint64 tickCount;
extern volatile uint64 preemptCount;

int main() {
    MemoryAllocator::kinit();
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);
    TCB::running = TCB::createThread(nullptr, nullptr, nullptr);

    thread_t a, b;
    thread_create(&a, spinA, nullptr);
    thread_create(&b, spinB, nullptr);

    // main is supervisor mode => never preempted => must yield explicitly
    while (!doneA || !doneB) { thread_dispatch(); }

    printStr("\ncounterA = "); printNum(counterA);
    printStr("\ncounterB = "); printNum(counterB);
    printStr("\nB progress seen at A's midpoint = "); printNum(bSeenByA);
    printStr(bSeenByA > 0 ? "  [PREEMPTION WORKS]\n" : "  [NO PREEMPTION]\n");

    printStr("\nticks = "); printNum(tickCount);
    printStr("\npreemptions = "); printNum(preemptCount);
    return 0;




//
////// ------------ MAIN FOR TESTING OFFICIAL TESTS (20 POINTS) ------------------
//    MemoryAllocator::kinit();
//    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);
//    TCB::running = TCB::createThread(nullptr, nullptr, nullptr); // main thread
//
//    // create a thread over userMain
//    thread_t userThread;
//    thread_create(&userThread, userMainWrapper, nullptr);
//
//    // let the userMain thread (and everything it spawns) run to completion
//    while (!((TCB*)userThread)->isFinished()) {
//        thread_dispatch();
//    }
//
//    return 0;

}

