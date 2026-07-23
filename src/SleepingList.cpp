//
// Created by os on 7/22/26.
//

#include "../h/SleepingList.hpp"
#include "../h/TCB.hpp"

void SleepingList::insert(TCB* thread, time_t time) {
    Elem* prev = nullptr;
    Elem* curr = head;

    // find our place in the list, each time deducting delta time
    while (curr != nullptr && time >= curr->data->sleepTime) {
        time -= curr->data->sleepTime;
        prev = curr;
        curr = curr->next;
    }

    Elem* elem = new Elem(thread, curr);
    if (prev != nullptr) {
        prev->next = elem;
    }
    else {
        head = elem;
    }

    thread->sleepTime = time; // our own delta time is relative to prev's time

    // update next threads sleep delta time relative to the new thread, others stay the same
    if (curr != nullptr) {
        curr->data->sleepTime -= time;
    }
}

void SleepingList::tick() {
    if (head != nullptr) {
        head->data->sleepTime--;
    }
}

TCB* SleepingList::removeExpired() {
    if (head == nullptr || head->data->sleepTime != 0) { return nullptr; }
    Elem* elem = head;
    head = head->next;
    TCB* ret = elem->data;
    delete elem;
    return ret;
}