//
// Created by mateusz on 25.11.2025.
//

#ifndef PROJEKT_SEMAPHORE_H
#define PROJEKT_SEMAPHORE_H
#include <sys/types.h>

#include "logger/Logger.h"

#define SEM_PERMS 0644

class Semaphore {
    public:
        static Semaphore attach(key_t key, Logger* log) {return Semaphore(key, log, false);};
        static Semaphore create(key_t key, Logger* log) {return Semaphore(key, log, true);};

        explicit Semaphore(key_t key, Logger* log, bool create = true, int initial_value = 1);
        ~Semaphore();

        // copying is prohibited!
        Semaphore(const Semaphore &) = delete;
        Semaphore & operator=(const Semaphore &) = delete;

        // but move is allowed
        Semaphore(Semaphore && other) noexcept : _semid(other._semid), _owner(other._owner), _log(other._log) {other._semid=-1;};
        Semaphore & operator=(Semaphore && other) noexcept;

        void lock() const;
        void unlock() const;
        [[nodiscard]] int value() const;

    private:
        bool _owner;
        Logger* _log;
        int _semid = -1;
};

#endif //PROJEKT_SEMAPHORE_H