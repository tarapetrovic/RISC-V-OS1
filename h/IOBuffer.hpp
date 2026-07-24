//
// Created by os on 7/23/26.
//

#ifndef PROJECT_BASE_V1_1_IOBUFFER_HPP
#define PROJECT_BASE_V1_1_IOBUFFER_HPP

#include "../lib/hw.h"
#include "_sem.hpp"


class InputBuffer {
public:
    InputBuffer() : head(0), tail(0), currentSize(0), itemAvailable(0) {}
    InputBuffer(const InputBuffer&) = delete;
    InputBuffer& operator=(const InputBuffer&) = delete;

    bool put(char c);   // interrupt side: never blocks, drops when full
    char get();         // user thread side: blocks when empty

private:
    static const int CAPACITY = 128;
    char data[CAPACITY];
    int head, tail, currentSize;
    KSemaphore itemAvailable;
};

class OutputBuffer {
public:
    OutputBuffer() : head(0), tail(0), currentSize(0),
                     itemAvailable(0), spaceAvailable(CAPACITY) {}
    OutputBuffer(const OutputBuffer&) = delete;
    OutputBuffer& operator=(const OutputBuffer&) = delete;

    void put(char c);   // user thread side: blocks when full
    char get();         // kernel output thread side: blocks when empty

    bool isEmpty() const { return currentSize == 0; }

private:
    static const int CAPACITY = 128;
    char data[CAPACITY];
    int head, tail, currentSize;
    KSemaphore itemAvailable;
    KSemaphore spaceAvailable;
};

#endif //PROJECT_BASE_V1_1_IOBUFFER_HPP
