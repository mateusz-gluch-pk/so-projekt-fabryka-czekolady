//
// Created by mateusz on 25.11.2025.
//

#ifndef PROJEKT_SHAREDMEMORY_H
#define PROJEKT_SHAREDMEMORY_H
#include <cstring>
#include <stdexcept>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_PERMS 0644

template<typename T>
class SharedMemory {
    public:
        explicit SharedMemory(key_t key, size_t size, Logger* log, bool create = true);
        ~SharedMemory() {detach();};

        // copying prohibited
        SharedMemory(const SharedMemory&) = delete;
        SharedMemory& operator=(const SharedMemory&) = delete;
        // moving allowed
        SharedMemory(SharedMemory&& other) noexcept;
        SharedMemory& operator=(SharedMemory&& other) noexcept;

        // get pointer to data
        T* get();
        T& operator*();
        T* operator->();

    private:
        void detach();

        Logger *_log;
        key_t _key;
        int _shmid = -1;
        bool _owner;;
        T* _data = nullptr;
};

template<typename T>
SharedMemory<T>::SharedMemory(key_t key, size_t size, Logger*log, bool create): _key(key), _log(log) {
    int flags = create ? (IPC_CREAT | IPC_EXCL | SHM_PERMS) : SHM_PERMS;

    _shmid = shmget(key, size, flags);
    if (_shmid == -1) {
        _log->fatal("Cannot create shared memory");
    }

    void* ptr = shmat(_shmid, nullptr, 0);
    if (ptr == reinterpret_cast<void *>(-1)) {
        _log->fatal("Cannot attach to shared memory %d", _shmid);
    }

    _data = static_cast<T*>(ptr);

    // initialize memory
    if (create) {
        std::memset(_data, 0, sizeof(T));
        _log->info("Initializing shared memory %d", _shmid);
    }
    _owner = create;
    _log->info("Created shared memory %d", _shmid);
}

template<typename T>
SharedMemory<T>::SharedMemory(SharedMemory &&other) noexcept:
    _key(other._key),
    _shmid(other._shmid),
    _owner(other._owner),
    _data(other._data) {
    other._shmid = -1;
    other._data = nullptr;
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
T * SharedMemory<T>::get() {
    _log->debug("Fetching pointer to shared memory %d data", _shmid);
    return _data;
}

template<typename T>
T & SharedMemory<T>::operator*() {
    _log->debug("Fetching pointer to shared memory %d data", _shmid);
    return *_data;
}

template<typename T>
T * SharedMemory<T>::operator->() {
    _log->debug("Fetching pointer to shared memory %d data", _shmid);
    return _data;
}

template<typename T>
void SharedMemory<T>::detach() {
    if (_data) {
        shmdt(_data);
        _log->info("Detached shared memory %d", _shmid);
        _data = nullptr;
    }

    if (_shmid != -1 && _owner) {
        shmctl(_shmid, IPC_RMID, nullptr);
        _log->info("Deleted shared memory %d", _shmid);
        _shmid = -1;
    }
}

#endif //PROJEKT_SHAREDMEMORY_H
