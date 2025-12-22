//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_SHAREDVECTOR_H
#define PROJEKT_SHAREDVECTOR_H

#include <stdexcept>
#include <nlohmann/json.hpp>

#include "Item.h"

template<typename T>
class SharedVector {
    public:
        SharedVector();
        explicit SharedVector(const int capacity): _data(new T[capacity]), _size(0), _capacity(capacity) {}
        void init(const int capacity);
        ~SharedVector();

        void push_back(const T &item);

        // overload [] - so that SharedVector won't hurt my eyes so much
        T& operator[](size_t index);

        const T& operator[](size_t index) const;

        // support "auto" range loops
        T* begin() { return _data; }
        T* end()   { return _data + _size; }

        const T* begin() const { return _data; }
        const T* end()   const { return _data + _size; }

        void erase(T* it);

        int size() const { return _size; }
        int capacity() const { return _capacity; }

        template<typename U>
        friend void from_json(const nlohmann::json &j, SharedVector<U> &vec);

        template<typename U>
        friend void to_json(nlohmann::json &j, const SharedVector<U> &vec);

    private:
        T* _data;
        size_t _size;
        size_t _capacity;
};

template<typename T>
SharedVector<T>::SharedVector(): _data(nullptr), _size(0), _capacity(0) {}

template<typename T>
void SharedVector<T>::init(const int capacity) {
    _data = new T[capacity];
    _capacity = capacity;
}

template<typename T>
SharedVector<T>::~SharedVector() {
    if (_data != nullptr) delete[] _data;
}

template<typename T>
void SharedVector<T>::push_back(const T &item) {
    if (_size >= _capacity) {
        throw std::runtime_error("vector reached full capacity");
    }
    _data[_size++] = item;
}

template<typename T>
T & SharedVector<T>::operator[](size_t index) {
    if (index >= _size) throw std::out_of_range("index out of range");
    return _data[index];
}

template<typename T>
const T & SharedVector<T>::operator[](size_t index) const {
    if (index >= _size) throw std::out_of_range("index out of range");
    return _data[index];
}

template<typename T>
void SharedVector<T>::erase(T *it) {
    // check range
    if (it < begin() || it >= end())
        throw std::out_of_range("iterator out of range");

    // move content one step back
    for (T* i = it + 1; i < end(); ++i) {
        *(i - 1) = *i;
    }

    // decrease nominal size
    --_size;
}

template<typename T>
void to_json(nlohmann::json& j, const SharedVector<T>& vec) {
    j = nlohmann::json::array();
    const T* data = vec.begin();
    for (size_t i = 0; i < vec.size(); ++i) {
        j.push_back(data[i]);
    }
}

template<typename T>
void from_json(const nlohmann::json& j, SharedVector<T>& vec) {
    if (!j.is_array())
        throw std::runtime_error("JSON is not an array for SharedVector");

    if (j.size() > vec.capacity())
        throw std::runtime_error("JSON array too large for SharedVector capacity");

    vec._size = j.size();
    T* data = vec.begin();

    for (size_t i = 0; i < vec._size; ++i) {
        data[i] = j[i].get<T>();
    }
}

#endif //PROJEKT_SHAREDVECTOR_H