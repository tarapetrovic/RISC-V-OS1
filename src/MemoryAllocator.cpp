//
// Created by os on 6/13/26.
//

#include "../h/MemoryAllocator.hpp"

MemoryAllocator::FreeMem *MemoryAllocator::freeMemHead = nullptr;

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
    if (ptr == nullptr) return -1; // error code for nullptr
    if ((const char*)ptr < (const char*)HEAP_START_ADDR || (const char*)ptr >= (const char*)HEAP_END_ADDR) return -2; // error code for out of bounds

    FreeMem* segToFree = (FreeMem*)((char*)ptr - sizeof(size_t));
    segToFree->size = *(size_t*)segToFree;

    FreeMem* curr = freeMemHead, *prev = nullptr;
    for (; curr != nullptr; prev = curr, curr = curr->next){
        if (curr > segToFree) break;
    }

    //need to check if we are double freeing - if the segment is already in free memory, eg overlapping with prev or curr memory addresses
    if (prev != nullptr && (char*)segToFree < (char*)prev + prev->size*MEM_BLOCK_SIZE) return -3;
    if (curr != nullptr && (char*)segToFree + segToFree->size*MEM_BLOCK_SIZE > (char*)curr) return -3;

    segToFree->next = curr;
    bool merged = false;

    if (curr && (char*)segToFree + segToFree->size*MEM_BLOCK_SIZE == (char*)curr) { // merge with right neighbor
        segToFree->size = segToFree->size + curr->size;
        segToFree->next = curr->next;
        if (prev) prev->next = segToFree;
        else freeMemHead = segToFree;
        merged = true;
    }

    if (prev && (char*)prev + prev->size*MEM_BLOCK_SIZE == (char*)segToFree) { // merge with left neighbor
        prev->size = prev->size + segToFree->size;
        prev->next = segToFree->next;
        merged = true;
    }

    if (!merged) { // didnt merge with any neighbours, needs to be inserted into the list
        if (prev) prev->next = segToFree;
        else freeMemHead = segToFree;
    }

    return 0;
}

void MemoryAllocator::kinit() {
    freeMemHead = (FreeMem*) HEAP_START_ADDR;
    freeMemHead->next = nullptr;
    freeMemHead->size = ((const char*) HEAP_END_ADDR - (const char*) HEAP_START_ADDR) / MEM_BLOCK_SIZE;
}
