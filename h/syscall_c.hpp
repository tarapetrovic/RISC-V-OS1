//
// Created by os on 6/20/26.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_C_HPP
#define PROJECT_BASE_V1_1_SYSCALL_C_HPP

#include "../lib/hw.h"


    void* mem_alloc(size_t size);
    int mem_free(void* ptr);
    void thread_dispatch();



#endif //PROJECT_BASE_V1_1_SYSCALL_C_HPP
