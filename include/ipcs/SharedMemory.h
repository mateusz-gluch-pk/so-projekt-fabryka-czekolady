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

#define SHM_PERMS 0644

template<typename T>
std::string type_name() {
    int status;
    std::unique_ptr<char[], void(*)(void*)> res{
        abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status),
        std::free
    };
    return (status == 0) ? res.get() : typeid(T).name();
}

template<typename T>
class SharedMemory {
    public:
        static SharedMemory attach(key_t key, size_t size, const Logger* log) {return SharedMemory(key, size, log, false);}
        static SharedMemory create(key_t key, size_t size, const Logger* log) {return SharedMemory(key, size, log, true);}

        explicit SharedMemory(key_t key, size_t size, const Logger* log, bool create = true);
        ~SharedMemory() {detach();};

        // copying prohibited
        SharedMemory(const SharedMemory&) = delete;
        SharedMemory& operator=(const SharedMemory&) = delete;
        // moving allowed
        SharedMemory(SharedMemory&& other) noexcept;
        SharedMemory& operator=(SharedMemory&& other) noexcept;

        // get pointer to data
        T* get() const;
        T& operator*() const;
        T* operator->() const;

    private:
        void detach();

        [[nodiscard]] std::string _msg(const std::string &msg) const {
            std::ostringstream ss;
            ss << _shmid  << "/" << type_name<T>();
            return "ipcs/SharedMemory/" + ss.str() + ":\t" + msg;
        }

        const Logger *_log;
        key_t _key;
        int _shmid = -1;
        bool _owner;
        T* _data = nullptr;
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
