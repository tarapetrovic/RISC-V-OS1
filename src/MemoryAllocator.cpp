//
// Created by os on 6/13/26.
//

#include "../h/MemoryAllocator.hpp"

void *MemoryAllocator::kmem_alloc(size_t size) {
    if (size == 0) return nullptr; // nothing needs to be allocated;
    FreeMem* curr = freeMemHead, *prev = nullptr;
    for (; curr != nullptr; prev = curr, curr = curr->next){
        if (curr->size >= size) break;
    }

    if (curr == nullptr) return nullptr; // no free space, cannot allocate - return error??

    FreeMem* oldNext = curr->next;
    void* allocatedSegment = curr;

    size_t remainingSize = curr->size - size;

    if (remainingSize > 0) { // if there is at least 1 block of free memory in this chunk left over after allocation
        curr = (FreeMem*) ((char*)curr + size*MEM_BLOCK_SIZE); // moving the pointer to new start of free memory
        curr->size = remainingSize;
        curr->next = oldNext;
        if(prev) prev->next = curr;
        else freeMemHead = curr;
    }
    else { // allocated the whole segment size
        if (prev) prev->next = oldNext;
        else freeMemHead = oldNext;
    }

    *(size_t*) allocatedSegment = size; // ovo mi je bilo bolno da skontam :(

    return (void*)((char*)allocatedSegment + sizeof(size_t));
}

int MemoryAllocator::kmem_free(void *ptr) {
    return -1;
}

void MemoryAllocator::kinit() {
    freeMemHead = (FreeMem*) HEAP_START_ADDR;
    freeMemHead->next = nullptr;
    freeMemHead->size = ((const char*) HEAP_END_ADDR - (const char*) HEAP_START_ADDR) / MEM_BLOCK_SIZE;
}
