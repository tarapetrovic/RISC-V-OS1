//
// Created by os on 6/20/26.
//

#include "../h/syscall_c.hpp"
#include "../lib/hw.h"

void* mem_alloc(size_t size) {
    volatile uint64 ret;
    __asm__ volatile ("mv a1, %[ulaz]" : : [ulaz] "r"(size));
    __asm__ volatile ("li a0, 0x01");
    __asm__ volatile ("ecall");
    __asm__ volatile ("mv %[izlaz], a0" : [izlaz] "=r"(ret));
    return (void*) ret;
}

int mem_free(void *ptr) {
    volatile uint64 ret;
    __asm__ volatile ("mv a1, %[ulaz]" : :[ulaz]"r"(ptr));
    __asm__ volatile ("li a0, 0x02");
    __asm__ volatile ("ecall");
    __asm__ volatile ("mv %[izlaz], a0" :[izlaz]"=r"(ret));
    return (int)ret;
}

void thread_dispatch() {
    __asm__ volatile ("li a0, 0x13");
    __asm__ volatile ("ecall");
}