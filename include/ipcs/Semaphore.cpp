//
// Created by mateusz on 25.11.2025.
//

#include "Semaphore.h"

#include <stdexcept>
#include <sys/sem.h>

Semaphore::Semaphore(const key_t key, const int initial_value) {
    _semid = semget(key, 1, IPC_CREAT | 0644);
    if (_semid == -1) {
        throw std::runtime_error("semget failed");
    }

    if (semctl(_semid, 0, SETVAL, initial_value) == -1) {
        throw std::runtime_error("semctl SETVAL failed");
    }
}

Semaphore::~Semaphore() {
    // if sem is valid - remove from system
    if (_semid != -1) {
        semctl(_semid, 0, IPC_RMID);
    }
}

Semaphore &Semaphore::operator=(Semaphore &&other) noexcept {
    if (this != &other) {
        _semid = other._semid;
        other._semid=-1;
    }
    return *this;
}

void Semaphore::lock() const {
    // lock semaphore operation:
    // semnum = 0 (only one)
    // semop = +1 - locked semaphore
    // flags - SEM_UNDO; unlock if process exits
    sembuf op = {0, +1, SEM_UNDO};
    if (semop(_semid, &op, 1) == -1) {
        throw std::runtime_error("semop lock failed");
    }
}

void Semaphore::unlock() const {
    // unlock semaphore operation:
    // semnum = 0 (only one)
    // semop = -1 - unlocked semaphore
    // flags - IPC_NOWAIT
    sembuf op = {0, -1, IPC_NOWAIT};
    if (semop(_semid, &op, 1) == -1) {
        throw std::runtime_error("semop unlock failed");
    }
}
