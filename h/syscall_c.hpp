//
// Created by os on 6/20/26.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_C_HPP
#define PROJECT_BASE_V1_1_SYSCALL_C_HPP

#include "../lib/hw.h"

    // typedef TCB *thread_t; // Andrejev nacin
    class _thread; // ovako traze specifikacije??? al onda mora da se castuje uvek?
    typedef _thread* thread_t;
    class _sem;
    typedef _sem* sem_t;

    void* mem_alloc(size_t size);
    int mem_free(void* ptr);
    void thread_dispatch();
    int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);
    int thread_exit();

    int sem_open(sem_t* handle, unsigned init);
    int sem_close(sem_t handle);
    int sem_wait(sem_t id);
    int sem_signal(sem_t id);
    int sem_wait_n(sem_t id, unsigned n);
    int sem_signal_n(sem_t id, unsigned n);


#endif //PROJECT_BASE_V1_1_SYSCALL_C_HPP
