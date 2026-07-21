//
// Created by os on 6/29/26.
//

#ifndef PROJECT_BASE_V1_1__SEM_HPP
#define PROJECT_BASE_V1_1__SEM_HPP

#include "List.hpp"
#include "TCB.hpp"
#include "MemoryAllocator.hpp"

class KSemaphore {

public:
    KSemaphore(unsigned init) : value(init), closed(false) {}
    int wait();
    int signal();
    int wait_n(unsigned n);
    int signal_n(unsigned n);
    int close();

    void* operator new(size_t size) {
        size_t blocks = (size + sizeof(size_t) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
        return MemoryAllocator::kmem_alloc(blocks);
    }
    void operator delete(void* ptr) {
        MemoryAllocator::kmem_free(ptr);
    }
private:
    unsigned value; // changed from int
    bool closed;
    List<TCB> blockedThreads;
    void block(unsigned n);
    void unblock();
};

#endif //PROJECT_BASE_V1_1__SEM_HPP
