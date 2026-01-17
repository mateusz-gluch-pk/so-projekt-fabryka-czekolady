//
// Created by mateusz on 25.11.2025.
//

#ifndef PROJEKT_SEMAPHORE_H
#define PROJEKT_SEMAPHORE_H
#include <sstream>
#include <sys/types.h>

#include "logger/Logger.h"

#define SEM_PERMS 0600

/**
 * @brief Wrapper around System V semaphores for interprocess synchronization.
 *
 * Supports creation, attachment, locking, unlocking, and querying the semaphore value.
 * This class uses **RAII** (Resource Acquisition Is Initialization):
 * the semaphore is automatically removed from the system when the owning
 * instance is destroyed. Ownership is tracked with the `_owner` flag:
 * - If `create=true`, the instance **owns** the semaphore and will remove it on destruction.
 * - If `create=false` (attach), the instance **does not own** the semaphore and will leave it in the system.
 *
 * Move semantics are supported, but copying is prohibited to avoid multiple owners.
 */
class Semaphore {
public:
    /**
     * @brief Attach to an existing semaphore.
     * @param key System V IPC key.
     * @param log Logger for reporting errors and info.
     * @return Semaphore object representing the attached semaphore.
     */
    static Semaphore attach(key_t key, Logger* log) { return Semaphore(key, log, false); }

    /**
     * @brief Create a new semaphore.
     * @param key System V IPC key.
     * @param log Logger for reporting errors and info.
     * @return Semaphore object representing the created semaphore.
     */
    static Semaphore create(key_t key, Logger* log) { return Semaphore(key, log, true); }

    /**
     * @brief Construct a semaphore.
     * @param key System V IPC key.
     * @param log Logger for reporting.
     * @param create Whether to create (true) or attach (false) the semaphore.
     * @param initial_value Initial value for the semaphore (only if creating).
     * @throw std::runtime_error via Logger::fatal if creation or initialization fails.
     */
    explicit Semaphore(key_t key, Logger* log, bool create = true, int initial_value = 1);

    /**
     * @brief Destructor removes semaphore from system if owner.
     * @throw std::runtime_error via Logger::fatal if deletion fails.
     */
    ~Semaphore();

    // copying is prohibited!
    Semaphore(const Semaphore &) = delete;
    Semaphore & operator=(const Semaphore &) = delete;

    // move semantics allowed
    Semaphore(Semaphore && other) noexcept : _semid(other._semid), _owner(other._owner), _log(other._log) {
        other._semid = -1;
    }
    Semaphore & operator=(Semaphore && other) noexcept;

    /**
     * @brief Lock the semaphore (decrement value).
     *        Blocks if the semaphore value is 0.
     * @throw std::runtime_error via Logger::fatal if locking fails.
     */
    void lock() const;

    /**
     * @brief Unlock the semaphore (increment value).
     * @throw std::runtime_error via Logger::fatal if unlocking fails.
     */
    void unlock() const;

    /**
     * @brief Get current value of semaphore.
     * @return Current semaphore value, or -1 if error occurred.
     */
    [[nodiscard]] int value() const;

private:
    /**
     * @brief Format a log message with semaphore ID.
     * @param msg Message text.
     * @return Formatted string.
     */
    [[nodiscard]] std::string _msg(const std::string &msg) const;

    bool _owner;     /**< True if this instance created the semaphore */
    Logger* _log;    /**< Logger for debug/error messages */
    int _semid = -1; /**< System V semaphore ID */
};

#endif //PROJEKT_SEMAPHORE_H