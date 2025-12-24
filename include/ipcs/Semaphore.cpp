//
// Created by mateusz on 25.11.2025.
//

#include "Semaphore.h"

#include <stdexcept>
#include <sys/sem.h>

#include "logger/Logger.h"

Semaphore::Semaphore(const key_t key, Logger *log, const bool create, const int initial_value): _log(log), _owner(create) {
    int flags = create ? (IPC_CREAT | IPC_EXCL | SEM_PERMS) : SEM_PERMS;
    _semid = semget(key, 1, flags);
    if (_semid == -1) {
        _log->fatal(_msg("Cannot create; errno: %d").c_str(), errno);
    }

    if (_owner) {
        if (semctl(_semid, 0, SETVAL, initial_value) == -1) {
            _log->fatal(_msg("Cannot set value %d; errno: %d").c_str(), initial_value, errno);
        }
        _log->info(_msg("Created with value %d and key %x").c_str(), initial_value, key);
    } else {
        _log->info(_msg("Acquired with key %x").c_str(), key);
    }

}

Semaphore::~Semaphore() {
    // if sem is valid - remove from system
    if (_semid != -1 && _owner) {
        _log->info(_msg("Deleted").c_str());
        const int ret = semctl(_semid, 0, IPC_RMID);
        if (ret == -1) {
            _log->fatal(_msg("Cannot delete; errno: %d").c_str(), errno);
        }
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
        _log->fatal(_msg("Cannot lock; errno: %d").c_str(), errno);
    }

    _log->debug(_msg("Locked").c_str());
}

void Semaphore::unlock() const {
    // unlock semaphore operation:
    // semnum = 0 (only one)
    // semop = +1 - unlock semaphore
    // flags - IPC_NOWAIT
    sembuf op = {0, +1, IPC_NOWAIT};
    if (semop(_semid, &op, 1) == -1) {
        _log->fatal(_msg("Cannot unlock; errno: %d").c_str(), errno);
    }

    _log->debug(_msg("Unlocked").c_str());
}

int Semaphore::value() const {
    // look up semaphore
    int v = semctl(_semid, 0, GETVAL);
    if (v == -1) {
        _log->error(_msg("Cannot get value").c_str());
    }
    return v;
}
