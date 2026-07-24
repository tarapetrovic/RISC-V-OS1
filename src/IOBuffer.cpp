//
// Created by os on 7/23/26.
//

#include "../h/IOBuffer.hpp"

bool InputBuffer::put(char c) {
    if (currentSize == CAPACITY) { return false; }   // drop the character- cant block here
    data[tail] = c;
    tail = (tail + 1) % CAPACITY;
    currentSize++;
    itemAvailable.signal(); // signal never suspends so its ok
    return true;
}

char InputBuffer::get() {
    itemAvailable.wait();
    char c = data[head];
    head = (head + 1) % CAPACITY;
    currentSize--;
    return c;
}

void OutputBuffer::put(char c) {
    spaceAvailable.wait();
    data[tail] = c;
    tail = (tail + 1) % CAPACITY;
    currentSize++;
    itemAvailable.signal();
}

char OutputBuffer::get() {
    itemAvailable.wait();
    char c = data[head];
    head = (head + 1) % CAPACITY;
    currentSize--;
    spaceAvailable.signal();
    return c;
}