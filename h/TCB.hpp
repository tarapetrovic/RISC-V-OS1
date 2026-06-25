//
// Created by os on 6/20/26.
//
// Kod preuzet sa 7. vezbi

#ifndef PROJECT_BASE_V1_1_TCB_HPP
#define PROJECT_BASE_V1_1_TCB_HPP

#include "../lib/hw.h"
#include "../h/Scheduler.hpp"

// Thread Control Block
class TCB
{
public:
    ~TCB() {} // destruktor ne treba da dealocira stek vec korisnik, takodje za brisanje niti je syscall thread_exit

    bool isFinished() const { return finished; } // za sad nek bude ovako, Jaksa ima enum stanja running finished blocked sleeping, mozda kasnije

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);
    using Argument = void*; // razmisli zelis li ovo da koristis

    static TCB *createThread(Body body, Argument arg, void* stack_space);

    static void yield();

    static TCB *running;

private:
    TCB(Body body, uint64 timeSlice, void* arg, void* stack_space) :
            body(body),
            stack(body != nullptr ? (char*) stack_space : nullptr), // main function has nullptr for its body, and it doesnt need stack
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64)((char*)stack + DEFAULT_STACK_SIZE) : 0 // na adresu stack dodamo default velicinu steka da dobijemo najvisu adresu, od koje zap pocinje stek pa raste na dole
                    }),
            timeSlice(timeSlice),
            arg(arg),
            finished(false)
    {
        if (body != nullptr) { Scheduler::put(this); } // check if its not main, then add it to scheduler
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void* stack; // da li je void* dobar tip? ili bolje char*?
    // uint64 *stack;
    void* arg; // ili Argument arg
    Context context;
    uint64 timeSlice;
    bool finished;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 timeSliceCounter;

    // static uint64 constexpr STACK_SIZE = 1024;
    // static uint64 constexpr TIME_SLICE = 2;
};

#endif //PROJECT_BASE_V1_1_TCB_HPP
