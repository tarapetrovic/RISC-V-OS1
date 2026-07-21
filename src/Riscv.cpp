//
// Created by os on 6/20/26.
//
// Kod preuzet sa 7. vezbi

#include "../h/Riscv.hpp"
// #include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.hpp"
#include "../h/_sem.hpp"


void Riscv::popSppSpie()
{
    ms_sstatus(SSTATUS_SPIE);
    if (TCB::running->isPrivileged())
        ms_sstatus(SSTATUS_SPP); // supervisor mode
    else
        mc_sstatus(SSTATUS_SPP); // user mode
    // mc_sip(SIP_SSIP);
    // __putc('Q');  // about to sret
    // __asm__ volatile ("addi sp, sp, 16"); // restore sp
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void printHex(uint64 val) {
    const char* hexDigits = "0123456789abcdef";
    __putc('0'); __putc('x');
    for (int shift = 60; shift >= 0; shift -= 4) {
        __putc(hexDigits[(val >> shift) & 0xF]);
    }
}
volatile uint64 extIrqCount = 0;

void Riscv::handleSupervisorTrap() {
    volatile uint64 scause = Riscv::r_scause();
    volatile uint64 sepc = Riscv::r_sepc();
    volatile uint64 sstatus = Riscv::r_sstatus();

    if (scause == 0x8000000000000001UL) {
        // timer/software interrupt — clear it and return for now
        mc_sip(SIP_SSIP);
        //return;
    }
    else if (scause == 0x8000000000000009UL)
    {
        // console_handler();
        int irq = plic_claim();
        if (irq == 0x0a) {
            console_handler();
        }
        plic_complete(irq);
    }

    // __putc('T'); printHex(scause); __putc('\n'); // confirms we even got here, and why
    else if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) // ecall iz korisnickog ili sistemskog rezima, respektivno
    {
        sepc += 4; // ecall returns pc on the old instruction, we need to shift it by 1 instruction, so 4 bytes

        volatile uint64 opNum, arg1, arg2, arg3, arg4; // saving the fun arguments on stack so they don't get lost
        __asm__ volatile("ld %[name], 10*8(fp)":[name] "=r" (opNum));
        __asm__ volatile("ld %[name], 11*8(fp)":[name] "=r" (arg1));
        __asm__ volatile("ld %[name], 12*8(fp)":[name] "=r" (arg2));
        __asm__ volatile("ld %[name], 13*8(fp)":[name] "=r" (arg3));
        __asm__ volatile("ld %[name], 14*8(fp)":[name] "=r" (arg4));

        // volatile uint64 retVal = 0;

        switch (opNum) {
            case 0x01: { // mem_alloc
                size_t size = (size_t) arg1;
                size_t blocks = (size + sizeof(size_t) + MEM_BLOCK_SIZE - 1) /
                                MEM_BLOCK_SIZE; // ceil for integers, finding number of blocks needed
                void *retVal = MemoryAllocator::kmem_alloc(blocks);
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal)); // we put retVal on stack on place of a0
                break;
            }
            case 0x02: { // mem_free
                uint64 retVal = MemoryAllocator::kmem_free((void *) arg1);
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal));
                break;
            }
            case 0x11: { // thread_create
                thread_t *handle = (thread_t *) arg1;
                TCB::Body body = (TCB::Body) arg2;
                void *targ = (void *) arg3;
                void *stack = (void *) arg4;
                TCB *tcb = TCB::createThread(body, targ, stack);
                if (handle && tcb) *handle = (thread_t) tcb;
                uint64 retVal = tcb ? 0 : -1;
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal));
                break;
            }
            case 0x12: { // thread_exit
                uint64 retVal = TCB::exit();
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal));
                break;
            }
            case 0x13: {
                TCB::dispatch();
                break;
            }
            case 0x21: { // sem_open
                // arg1 - handle, arg2 - init
                KSemaphore *newSem = new KSemaphore(arg2);
                if (newSem) {
                    KSemaphore **handle = (KSemaphore **) arg1;
                    *handle = newSem;
                    __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(0));
                } else {
                    __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(-1));
                }
                break;
            }
            case 0x22: { // sem_close
                // arg1 - handle
                uint64 retVal;
                KSemaphore *handle = (KSemaphore *) arg1;
                retVal = handle->close();
                if (retVal == 0) {
                    delete handle; // deallocate semaphore, blocked thread will unblock with wakeupError
                }
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal));
                break;
            }
            case 0x23: { // sem_wait
                // arg1 - handle
                KSemaphore *handle = (KSemaphore *) arg1;
                uint64 retVal = handle->wait();
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal));
                break;
            }
            case 0x24: { // sem_signal
                // arg1 - handle
                KSemaphore *handle = (KSemaphore *) arg1;
                uint64 retVal = handle->signal();
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal));
                break;
            }
            case 0x25: { // sem_wait_n
                // arg1 - handle, arg2 - n
                KSemaphore *handle = (KSemaphore *) arg1;
                uint64 retVal = handle->wait_n((unsigned) arg2);
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal));
                break;
            }
            case 0x26: { // sem_signal_n
                // arg1 - handle, arg2 - n
                KSemaphore *handle = (KSemaphore *) arg1;
                uint64 retVal = handle->signal_n((unsigned) arg2);
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal));
                break;
            }
            case 0x41: { // getc, temporary for testing
                char retVal = __getc();
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal));
                break;
            }
            case 0x42: { // putc, temporary for testing
                char ch = (char) arg1;
                __putc(ch);
                break;
            }
            default:
                __putc('?');
        }

    }
    else {
        extIrqCount++;
    }

    w_sstatus(sstatus); // reconstruct registers
    w_sepc(sepc); // reconstruct registers
}
