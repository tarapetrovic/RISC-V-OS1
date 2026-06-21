//
// Created by os on 6/20/26.
//
// Kod preuzet sa 7. vezbi

#ifndef PROJECT_BASE_V1_1_TCB_HPP
#define PROJECT_BASE_V1_1_TCB_HPP

#include "../lib/hw.h"
// #include "scheduler.hpp"
/*
// Thread Control Block
class TCB
{
public:
    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; } // za sad nek bude ovako, Jaksa ima enum stanja running finished blocked sleeping, mozda kasnije

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);
    using Argument = void*;

    static TCB *createThread(Body body, Argument arg, vpid* stack_space);

    static void yield();

    static TCB *running;

    // do ovde sam stigla sa proverom sta mi sve treba!! - zato je ostatak u komentaru, vraticu se na ovo
// ----------------------------------------------------------------------

private:
    TCB(Body body, uint64 timeSlice) :
            body(body),
            stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[STACK_SIZE] : 0
                    }),
            timeSlice(timeSlice),
            finished(false)
    {
        if (body != nullptr) { Scheduler::put(this); }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = 1024;
    static uint64 constexpr TIME_SLICE = 2;
};
*/

#endif //PROJECT_BASE_V1_1_TCB_HPP
