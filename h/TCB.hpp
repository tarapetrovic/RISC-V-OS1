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

    bool isPrivileged() const { return privileged; }

    bool isBlocked() const { return blocked; }

    void setBlocked(bool value) { blocked = value; }

    void setWaitingCount(unsigned value) { waitingCount = value; } // do i really need getters and setters if tcb and semaphores are friends???

    unsigned getWaitingCount() { return waitingCount; }

    void setWakeupError(bool value) { wakeupError = value; }

    bool getWakeupError() { return wakeupError; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);
    using Argument = void*; // razmisli zelis li ovo da koristis

    static TCB *createThread(Body body, Argument arg, void* stack_space, bool privileged = false);

    static void yield();
    static void dispatch();

    static TCB *running;

private:
    TCB(Body body, uint64 timeSlice, void* arg, void* stack_space, bool privileged) :
            body(body),
            stack(body != nullptr ? (char*) stack_space : nullptr), // main function has nullptr for its body, and it doesnt need stack
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64)((char*)stack + DEFAULT_STACK_SIZE) : 0 // na adresu stack dodamo default velicinu steka da dobijemo najvisu adresu, od koje zap pocinje stek pa raste na dole
                    }),
            timeSlice(timeSlice),
            arg(arg),
            finished(false),
            blocked(false),
            // privileged(body == nullptr),
            privileged(privileged),
            waitingCount(0),
            wakeupError(0),
            sleepTime(0)
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
    Context context;
    uint64 timeSlice;
    void* arg; // ili Argument arg
    bool finished;
    bool blocked;
    bool privileged;
    unsigned waitingCount;
    bool wakeupError;
    time_t sleepTime;

    friend class Riscv;
    friend class KSemaphore;
    friend class SleepingList;
    // friend class Scheduler; // i moved dispatch to public, think about if this was a good idea!!

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    // static void dispatch(); // moved it to public instead of private as a lot of classes needed it, to avoid a lot of friend classes

    static int exit();

    static uint64 timeSliceCounter;

    // static uint64 constexpr STACK_SIZE = 1024;
    // static uint64 constexpr TIME_SLICE = 2;
};

#endif //PROJECT_BASE_V1_1_TCB_HPP
