//
// Created by os on 7/23/26.
//

#ifndef PROJECT_BASE_V1_1_KCONSOLE_HPP
#define PROJECT_BASE_V1_1_KCONSOLE_HPP

#include "IOBuffer.hpp"
#include "MemoryAllocator.hpp"

class KConsole {
public:
    static KConsole* getInstance();

    void putc(char c) { outputBuffer.put(c); }
    char getc() { return inputBuffer.get(); }

    // called from the trap handler on a console interrupt: drains RX
    void handleInterrupt();

    // body of the privileged kernel thread that feeds the controller
    static void outputThreadBody(void*);

    // checks if output buffer is empty, for flushing at the end
    bool outputEmpty() const { return outputBuffer.isEmpty(); }

    void* operator new(size_t size) {
        size_t blocks = (size + sizeof(size_t) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
        return MemoryAllocator::kmem_alloc(blocks);
    }
    void operator delete(void* ptr) {
        MemoryAllocator::kmem_free(ptr);
    }

private:
    KConsole() {}
    static KConsole* instance;
    InputBuffer inputBuffer;
    OutputBuffer outputBuffer;
};


#endif //PROJECT_BASE_V1_1_KCONSOLE_HPP
