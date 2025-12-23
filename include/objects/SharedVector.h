//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_SHAREDVECTOR_H
#define PROJEKT_SHAREDVECTOR_H

#include <stdexcept>
#include <nlohmann/json.hpp>

#include "Item.h"

template<typename T, size_t Capacity>
class SharedVector {
    public:
        // SharedVector();
        // explicit SharedVector(const int capacity): _data(new T[capacity]), _size(0), _capacity(capacity) {}
        // void init(const int capacity);
        // ~SharedVector();

        void push_back(const T &item);

        // overload [] - so that SharedVector won't hurt my eyes so much
        T& operator[](size_t index);

        const T& operator[](size_t index) const;

        // support "auto" range loops
        T* begin() { return data; }
        T* end()   { return data + size; }

        const T* begin() const { return data; }
        const T* end()   const { return data + size; }

        void erase(T* it);

        template<typename U, size_t JCapacity>
        friend void from_json(const nlohmann::json &j, SharedVector<U, JCapacity> &vec);

        template<typename U, size_t JCapacity>
        friend void to_json(nlohmann::json &j, const SharedVector<U, JCapacity> &vec);

        T data[Capacity];
        size_t size;
        size_t capacity = Capacity;
};

// template<typename T>
// SharedVector<T>::SharedVector(): _data(nullptr), _size(0), _capacity(0) {}

// template<typename T>
// void SharedVector<T>::init(const int capacity) {
//     _data = new T[capacity];
//     _capacity = capacity;
// }

// template<typename T>
// SharedVector<T>::~SharedVector() {
//     if (_data != nullptr) delete[] _data;
// }

template<typename T, size_t Capacity>
void SharedVector<T, Capacity>::push_back(const T &item) {
    if (size >= capacity) {
        throw std::runtime_error("vector reached full capacity");
    }
    data[size++] = item;
}

template<typename T, size_t Capacity>
T & SharedVector<T, Capacity>::operator[](size_t index) {
    if (index >= size) throw std::out_of_range("index out of range");
    return data[index];
}

template<typename T, size_t Capacity>
const T & SharedVector<T, Capacity>::operator[](size_t index) const {
    if (index >= size) throw std::out_of_range("index out of range");
    return data[index];
}

template<typename T, size_t Capacity>
void SharedVector<T, Capacity>::erase(T *it) {
    // check range
    if (it < begin() || it >= end())
        throw std::out_of_range("iterator out of range");

    // move content one step back
    for (T* i = it + 1; i < end(); ++i) {
        *(i - 1) = *i;
    }

    // decrease nominal size
    --size;
}

template<typename T, size_t Capacity>
void to_json(nlohmann::json& j, const SharedVector<T, Capacity>& vec) {
    j = nlohmann::json::array();
    const T* data = vec.begin();
    for (size_t i = 0; i < vec.size; ++i) {
        j.push_back(data[i]);
    }
}

template<typename T, size_t Capacity>
void from_json(const nlohmann::json& j, SharedVector<T, Capacity>& vec) {
    if (!j.is_array())
        throw std::runtime_error("JSON is not an array for SharedVector");

    if (j.size() > vec.capacity)
        throw std::runtime_error("JSON array too large for SharedVector capacity");

    vec.size = j.size();
    T* data = vec.begin();

    for (size_t i = 0; i < vec.size; ++i) {
        data[i] = j[i].get<T>();
    }
}

#endif //PROJEKT_SHAREDVECTOR_H