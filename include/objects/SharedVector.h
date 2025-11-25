//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_SHAREDVECTOR_H
#define PROJEKT_SHAREDVECTOR_H

#include <stdexcept>

template<typename T>
class SharedVector {
    public:
        size_t _size{};
        size_t _capacity{};

        void init(size_t capacity) {
            _capacity = capacity;
            _size = 0;
        }

        // overload [] - so that SharedVector won't hurt my eyes so much
        T& operator[](size_t index) {
            if (index >= _size) throw std::out_of_range("index out of range");
            return reinterpret_cast<T*>(this + 1)[index];
        }

        const T& operator[](size_t index) const {
            if (index >= _size) throw std::out_of_range("index out of range");
            return reinterpret_cast<const T*>(this + 1)[index];
        }

        // support "auto" range loops
        T* begin() { return reinterpret_cast<T*>(this + 1); }
        T* end()   { return begin() + _size; }

        const T* begin() const { return reinterpret_cast<const T*>(this + 1); }
        const T* end() const   { return begin() + _size; }

        void push_back(const T& item) {
            if (_size >= _capacity) throw std::runtime_error("full");
            this[_size++] = item;
        }

        void erase(size_t index) {
            if (index >= _size) throw std::out_of_range("index out of range");
            for (size_t i = index + 1; i < _size; ++i)
                this[i - 1] = this()[i];
            --_size;
        }
};


#endif //PROJEKT_SHAREDVECTOR_H