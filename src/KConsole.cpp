//
// Created by os on 7/23/26.
//

#include "../h/KConsole.hpp"
#include "../h/TCB.hpp"

KConsole* KConsole::instance = nullptr;

KConsole* KConsole::getInstance() {
    if (instance == nullptr) {
        instance = new KConsole();
    }
    return instance;
}

void KConsole::handleInterrupt() {
    while (*((char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT) { // loop while status bit is 1 (item available) and transfer data into buffer
        inputBuffer.put(*((char*)CONSOLE_RX_DATA));   // drops if full; RX -> receive - bit 0 in status
    }
}

void KConsole::outputThreadBody(void*) {
    KConsole* console = getInstance();
    while (true) {
        char c = console->outputBuffer.get();   // block until a char exists, then get data when available
        // only then poll TX-readiness — no blocking call inside this loop
        while (!(*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT)) { // TX -> transmit - bit 5 in status
            TCB::dispatch(); // wait until TX bit in status is set
        }
        *((char*)CONSOLE_TX_DATA) = c;
    }
}
