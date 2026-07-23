//
// Created by os on 6/24/26.
//
// Kod preuzet sa 7. vezbi

#ifndef PROJECT_BASE_V1_1_SCHEDULER_HPP
#define PROJECT_BASE_V1_1_SCHEDULER_HPP

#include "../lib/hw.h"
#include "../h/List.hpp"
#include "../h/SleepingList.hpp"

class TCB;

class Scheduler
{
private:
    static List<TCB> readyThreadQueue;
    static SleepingList sleepingThreadQueue;

public:
    static TCB *get();
    static void put(TCB *ccb);

    static int putToSleep(time_t time);
    static void updateSleeping();

};

#endif //PROJECT_BASE_V1_1_SCHEDULER_HPP
