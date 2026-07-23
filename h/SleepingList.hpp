//
// Created by os on 7/22/26.
//

#ifndef PROJECT_BASE_V1_1_SLEEPINGLIST_HPP
#define PROJECT_BASE_V1_1_SLEEPINGLIST_HPP

#include "../lib/hw.h"
#include "../h/MemoryAllocator.hpp"

class TCB; // forward declaration - we're avoiding circular include (sleepinglist - tcb - scheduler - sleepinglist)

class SleepingList {
public:
    SleepingList() : head(nullptr) {}
    SleepingList(const SleepingList&) = delete;
    SleepingList& operator=(const SleepingList&) = delete;

    // insert thread to be woken 'time' ticks from now (time > 0)
    void insert(TCB* thread, time_t time);

    // one timer period elapsed- decrement only the head's remaining time
    void tick();

    // pop the next thread whose time has expired, or nullptr if none
    TCB* removeExpired();

private:
    struct Elem {
        TCB* data;
        Elem* next;
        Elem(TCB* data, Elem* next) : data(data), next(next) {}

        // allocate through the kernel allocator directly, never via the
        // mem_alloc syscall — removeExpired() runs inside the timer interrupt
        void* operator new(size_t size) {
            size_t blocks = (size + sizeof(size_t) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
            return MemoryAllocator::kmem_alloc(blocks);
        }
        void operator delete(void* ptr) {
            MemoryAllocator::kmem_free(ptr);
        }
    };

    Elem* head;   // no tail: this list never appends at the end
};


#endif //PROJECT_BASE_V1_1_SLEEPINGLIST_HPP
