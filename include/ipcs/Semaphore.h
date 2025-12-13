//
// Created by mateusz on 25.11.2025.
//

#ifndef PROJEKT_SEMAPHORE_H
#define PROJEKT_SEMAPHORE_H
#include <sys/types.h>

#include "logger/Logger.h"

#define SEM_PERMS 0644

// union SemaphoreUnion {
//     int val;
//     struct semid_ds* buf;
//     unsigned short* array;
// };

class Semaphore {
    public:
        explicit Semaphore(key_t key, Logger* log, int initial_value = 1);
        ~Semaphore();

        // copying is prohibited!
        Semaphore(const Semaphore &) = delete;
        Semaphore & operator=(const Semaphore &) = delete;
        // but move is allowed
        Semaphore(Semaphore && other) noexcept : _semid(other._semid) {other._semid=-1;};
        Semaphore & operator=(Semaphore && other) noexcept;

        void lock() const;
        void unlock() const;

    private:
        Logger* _log;
        int _semid = -1;
};

#endif //PROJEKT_SEMAPHORE_H