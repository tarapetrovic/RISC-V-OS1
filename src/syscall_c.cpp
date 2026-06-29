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

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg) {
    volatile uint64 ret;
    // void(*arg2)(void *) = start_routine;
    void* stack = (char*) (new char[DEFAULT_STACK_SIZE]); // moze i samo mem_alloc(DEFAULT_STACK_SIZE), svejedno je valjda
    if (!stack) return -1;
    __asm__ volatile ("mv a4, %[ulaz]" : :[ulaz]"r"(stack));
    __asm__ volatile ("mv a3, %[ulaz]" : :[ulaz]"r"(arg));
    __asm__ volatile ("mv a2, %[ulaz]" : :[ulaz]"r"(start_routine));
    __asm__ volatile ("mv a1, %[ulaz]" : :[ulaz]"r"(handle));
    __asm__ volatile ("li a0, 0x11");
    __asm__ volatile ("ecall");
    __asm__ volatile ("mv %[izlaz], a0" :[izlaz]"=r"(ret));
    return (int)ret;
}

int thread_exit() {
    volatile uint64 ret;
    __asm__ volatile ("li a0, 0x12");
    __asm__ volatile ("ecall");
    __asm__ volatile ("mv %[izlaz], a0" :[izlaz]"=r"(ret));
    return (int)ret;
}

void thread_dispatch() {
    __asm__ volatile ("li a0, 0x13");
    __asm__ volatile ("ecall");
}