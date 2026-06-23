//
// Created by os on 6/20/26.
//
// Kod preuzet sa 7. vezbi

#include "../h/TCB.hpp"

TCB *TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(TCB::Body body, TCB::Argument arg, void *stack_space) {
    return nullptr;
}


void TCB::yield() {

}

void TCB::threadWrapper() {

}


void TCB::contextSwitch(TCB::Context *oldContext, TCB::Context *runningContext) {

}


void TCB::dispatch() {

}