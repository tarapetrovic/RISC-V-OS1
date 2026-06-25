//
// Created by os on 6/20/26.
//
// Kod preuzet sa 7. vezbi

#include "../h/Riscv.hpp"
// #include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/MemoryAllocator.hpp"


void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}


void Riscv::handleSupervisorTrap() {
    volatile uint64 scause = Riscv::r_scause();
    volatile uint64 sepc = Riscv::r_sepc();
    volatile uint64 sstatus = Riscv::r_sstatus();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) // ecall iz korisnickog ili sistemskog rezima, respektivno
    {
        sepc += 4; // ecall returns pc on the old instruction, we need to shift it by 1 instruction, so 4 bytes

        volatile uint64 opNum, arg1, arg2, arg3, arg4; // saving the fun arguments on stack so they don't get lost
        __asm__ volatile("ld %[name], 10*8(fp)":[name] "=r" (opNum));
        __asm__ volatile("ld %[name], 11*8(fp)":[name] "=r" (arg1));
        __asm__ volatile("ld %[name], 12*8(fp)":[name] "=r" (arg2));
        __asm__ volatile("ld %[name], 13*8(fp)":[name] "=r" (arg3));
        __asm__ volatile("ld %[name], 14*8(fp)":[name] "=r" (arg4));

        volatile uint64 retVal = 0;

        switch (opNum) {
            case 0x01: {// mem_alloc
                size_t size = (size_t) arg1;
                size_t blocks = (size + sizeof(size_t) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE; // ceil for integers, finding number of blocks needed
                void *ret = MemoryAllocator::kmem_alloc(blocks);
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(ret)); // we put ret on stack on place of a0
                break;
            }
            case 0x02: // mem_free
                retVal = MemoryAllocator::kmem_free((void*) arg1);
                __asm__ volatile ("sd %[ulaz], 10*8(fp)" : : [ulaz]"r"(retVal));
                break;
        }

    }
    w_sstatus(sstatus); // reconstruct registers
    w_sepc(sepc); // reconstruct registers
}
