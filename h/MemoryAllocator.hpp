//
// Created by os on 6/13/26.
//

#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
private:
    MemoryAllocator() = default;

public:
    MemoryAllocator(const MemoryAllocator& obj) = delete;
    MemoryAllocator(MemoryAllocator&& obj) = delete;


    struct FreeMem {
        size_t size; // size in blocks
        FreeMem* next = nullptr;
    };

    static FreeMem* freeMemHead;
    static void* kmem_alloc(size_t size);
    static int kmem_free(void* ptr);
    static void kinit();

};



#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
