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
        explicit SharedMemory(key_t key, size_t size, bool create = true): _key(key) {
            _shmid = shmget(key, size, IPC_CREAT | SHM_PERMS);
            if (_shmid == -1) {
                throw std::runtime_error("Error creating shared memory");
            }

            void* ptr = shmat(_shmid, nullptr, 0);
            if (ptr == reinterpret_cast<void *>(-1)) {
                throw std::runtime_error("Error attaching shared memory");
            }

            _data = static_cast<T*>(ptr);

            // initialize memory
            if (create) {
                std::memset(_data, 0, sizeof(T));
            }
            _owner = create;
        };

        // copying prohibited
        SharedMemory(const SharedMemory&) = delete;
        SharedMemory& operator=(const SharedMemory&) = delete;

        // moving allowed
        SharedMemory(SharedMemory&& other) noexcept: _key(other._key), _shmid(other._shmid), _data(other._data) {
            other._shmid = -1;
            other._data = nullptr;
        };

        SharedMemory& operator=(SharedMemory&& other) noexcept {
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
        };

        ~SharedMemory() {detach();};

        // get pointer to data
        T* get() {return _data;};
        T& operator*() {return *_data;};
        T* operator->() {return _data;};

    private:
        void detach() {
            if (_data) {
                shmdt(_data);
                _data = nullptr;
            }

            if (_shmid != -1 && _owner) {
                shmctl(_shmid, IPC_RMID, nullptr);
                _shmid = -1;
            }
        }

        key_t _key;
        int _shmid = -1;
        bool _owner;;
        T* _data = nullptr;
};


#endif //PROJEKT_SHAREDMEMORY_H