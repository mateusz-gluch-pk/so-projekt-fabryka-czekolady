//
// Created by mateusz on 25.11.2025.
//

#ifndef PROJEKT_SHAREDMEMORY_H
#define PROJEKT_SHAREDMEMORY_H
#include <cstring>
#include <sys/shm.h>
#include <sys/types.h>

#include <typeinfo>
#include <cxxabi.h>
#include <memory>

#include "logger/Logger.h"

#define SHM_PERMS 0600

template<typename T>
std::string type_name() {
    int status;
    std::unique_ptr<char[], void(*)(void*)> res{
        abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status),
        std::free
    };
    return (status == 0) ? res.get() : typeid(T).name();
}

/**
 * @brief RAII wrapper for System V shared memory segments.
 *
 * This class uses **RAII** (Resource Acquisition Is Initialization):
 * the semaphore is automatically removed from the system when the owning
 * instance is destroyed.
 *
 * This class manages a shared memory segment with automatic cleanup:
 * - If `create=true`, the instance **owns** the memory and will remove it on destruction.
 * - If `create=false` (attach), the instance **does not own** the memory and will leave it in the system.
 *
 * Memory is attached on construction and detached automatically in the destructor.
 * Move semantics are supported, but copying is prohibited to avoid multiple owners.
 *
 * @tparam T Type of object stored in shared memory.
 */
template<typename T>
class SharedMemory {
public:
    /**
     * @brief Attach to an existing shared memory segment.
     * @param key System V IPC key.
     * @param size Size of memory in bytes.
     * @param log Logger for reporting errors and info.
     * @return SharedMemory object representing the attached segment.
     */
    static SharedMemory attach(key_t key, size_t size, const Logger* log) {
        return SharedMemory(key, size, log, false);
    }

    /**
     * @brief Create a new shared memory segment.
     * @param key System V IPC key.
     * @param size Size of memory in bytes.
     * @param log Logger for reporting errors and info.
     * @return SharedMemory object representing the created segment.
     */
    static SharedMemory create(key_t key, size_t size, const Logger* log) {
        return SharedMemory(key, size, log, true);
    }

    /**
     * @brief Construct and attach/create a shared memory segment.
     * @param key System V IPC key.
     * @param size Size in bytes.
     * @param log Logger for reporting.
     * @param create If true, create new memory; if false, attach to existing.
     * @throw std::runtime_error via Logger::fatal if creation or attachment fails.
     */
    explicit SharedMemory(key_t key, size_t size, const Logger* log, bool create = true);

    /**
     * @brief Destructor detaches memory and removes it if owned.
     */
    ~SharedMemory() { detach(); }

    // Copying prohibited
    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;

    // Move allowed
    SharedMemory(SharedMemory&& other) noexcept;
    SharedMemory& operator=(SharedMemory&& other) noexcept;

    /**
     * @brief Get pointer to the shared memory data.
     * @return Pointer to memory of type T.
     */
    T* get() const;

    /**
     * @brief Dereference operator for accessing the stored object.
     * @return Reference to object stored in shared memory.
     */
    T& operator*() const;

    /**
     * @brief Arrow operator for accessing members of the stored object.
     * @return Pointer to object stored in shared memory.
     */
    T* operator->() const;

private:
    /**
     * @brief Detach from memory and remove if owner.
     */
    void detach();

    /**
     * @brief Format a log message with memory ID and type.
     * @param msg Message text.
     * @return Formatted string.
     */
    [[nodiscard]] std::string _msg(const std::string &msg) const {
        std::ostringstream ss;
        ss << _shmid  << "/" << type_name<T>();
        return "ipcs/SharedMemory/" + ss.str() + ":\t" + msg;
    }

    const Logger* _log; /**< Logger for debug/info/error messages */
    key_t _key;         /**< System V IPC key */
    int _shmid = -1;    /**< System V shared memory ID */
    bool _owner;        /**< True if this instance created the memory */
    T* _data = nullptr; /**< Pointer to attached memory */
};

template<typename T>
SharedMemory<T>::SharedMemory(key_t key, size_t size, const Logger *log, bool create):
    _key(key),
    _log(log),
    _owner(create) {
    const int flags = _owner ? (IPC_CREAT | IPC_EXCL | SHM_PERMS) : SHM_PERMS;
    _shmid = shmget(key, size, flags);
    if (_shmid == -1) {
        _log->fatal(_msg("Cannot create; errno: %d").c_str(), errno);
    }

    void *ptr = shmat(_shmid, nullptr, 0);
    if (ptr == reinterpret_cast<void *>(-1)) {
        _log->fatal(_msg("Cannot attach; errno: %d").c_str(), errno);
    }

    _data = static_cast<T *>(ptr);
    // initialize memory
    if (_owner) {
        _log->info(_msg("Initializing").c_str());
        std::memset(_data, 0, size);

        _log->info(_msg("Created with key %x").c_str(), key);
    } else {
        _log->info(_msg("Acquired with key %x").c_str(), key);
    }
}

template<typename T>
SharedMemory<T>::SharedMemory(SharedMemory &&other) noexcept:
    _key(other._key),
    _shmid(other._shmid),
    _owner(other._owner),
    _data(other._data),
    _log(other._log) {
    other._shmid = -1;
    other._data = nullptr;
    other._log = nullptr;
}

template<typename T>
SharedMemory<T> & SharedMemory<T>::operator=(SharedMemory &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    detach();
    _key = other._key;
    _shmid = other._shmid;
    _data = other._data;
    other._shmid = -1;
    other._data = nullptr;
    return *this;
}

template<typename T>
T * SharedMemory<T>::get() const {
    _log->debug(_msg("Fetching pointer").c_str());
    return _data;
}

template<typename T>
T & SharedMemory<T>::operator*() const {
    _log->debug(_msg("Fetching pointer").c_str());
    return *_data;
}

template<typename T>
T * SharedMemory<T>::operator->() const {
    _log->debug(_msg("Fetching pointer").c_str());
    return _data;
}

template<typename T>
void SharedMemory<T>::detach() {
    if (_data) {
        shmdt(_data);
        _log->info(_msg("Detached").c_str());
        _data = nullptr;
    }

    if (_shmid != -1 && _owner) {
        shmctl(_shmid, IPC_RMID, nullptr);
        _log->info(_msg("Deleted").c_str());
        _shmid = -1;
    }
}

#endif //PROJEKT_SHAREDMEMORY_H
