//
// Created by mateusz on 25.11.2025.
//

#include "Semaphore.h"

#include <stdexcept>
#include <sys/sem.h>

#include "logger/Logger.h"

Semaphore::Semaphore(const key_t key, Logger *log, const bool create, const int initial_value): _log(log), _owner(create) {
    int flags = IPC_CREAT | SEM_PERMS;
    if (create) flags |= IPC_EXCL;
    _semid = semget(key, 1, flags);
    if (_semid == -1) {
        _log->fatal("Failed to create semaphore");
    }

    if (_owner) {
        if (semctl(_semid, 0, SETVAL, initial_value) == -1) {
            _log->fatal("Failed to set semaphore value");
        }
        _log->info("Semaphore %d created with value %d and key %x", _semid, initial_value, key);
    } else {
        _log->info("Acquired semaphore %d with key %x", _semid, key);
    }

}

Semaphore::~Semaphore() {
    // if sem is valid - remove from system
    if (_semid != -1 && _owner) {
        _log->info("Semaphore %d destroyed", _semid);
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
    // semop = -1 - lock semaphore
    // flags - SEM_UNDO; unlock if process exits
    sembuf op = {0, -1, SEM_UNDO};
    if (semop(_semid, &op, 1) == -1) {
        _log->fatal("Failed to lock semaphore %d", _semid);
    }

    _log->debug("Semaphore %d locked", _semid);
}

void Semaphore::unlock() const {
    // unlock semaphore operation:
    // semnum = 0 (only one)
    // semop = +1 - unlock semaphore
    // flags - IPC_NOWAIT
    sembuf op = {0, +1, IPC_NOWAIT};
    if (semop(_semid, &op, 1) == -1) {
        _log->fatal("Failed to unlock semaphore %d", _semid);
    }

    _log->debug("Semaphore %d unlocked", _semid);
}

int Semaphore::value() const {
    // look up semaphore
    int v = semctl(_semid, 0, GETVAL);
    if (v == -1) {
        _log->error("Failed to get semaphore %d value", _semid);
    }
    return v;
}
