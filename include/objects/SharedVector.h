//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_SHAREDVECTOR_H
#define PROJEKT_SHAREDVECTOR_H

#include <stdexcept>
#include <nlohmann/json.hpp>

#include "Item.h"

/**
 * @brief Fixed-capacity vector with shared semantics. (Fixed size necessary for Shared Memory)
 *
 * @tparam T Type of elements stored.
 * @tparam Capacity Maximum number of elements.
 */
template<typename T, size_t Capacity>
class SharedVector {
    public:
        /**
         * @brief Adds an item to the end of the vector.
         * @param item Item to add.
         * @throw std::runtime_error if vector is full.
         */
        void push_back(const T &item);

        /**
         * @brief Access element by index (modifiable).
         * @param index Index of element.
         * @return Reference to element.
         * @throw std::out_of_range if index >= size.
         */
        T& operator[](size_t index);

        /**
         * @brief Access element by index (const version).
         * @param index Index of element.
         * @return Const reference to element.
         * @throw std::out_of_range if index >= size.
         */
        const T& operator[](size_t index) const;

        /**
         * @brief Returns iterator to first element. Necessary to support foreach loops.
         * @return Pointer to first element.
         */
        T* begin() { return data; }

        /**
         * @brief Returns iterator past the last element.
         * @return Pointer past the last element.
         */
        T* end()   { return data + size; }

        /**
         * @brief Const iterator to first element.
         * @return Const pointer to first element.
         */
        const T* begin() const { return data; }

        /**
         * @brief Const iterator past the last element.
         * @return Const pointer past the last element.
         */
        const T* end()   const { return data + size; }

        /**
         * @brief Erases element at given iterator.
         * @param it Pointer to element to erase.
         * @throw std::out_of_range if iterator is invalid.
         */
        void erase(T* it);

        /**
         * @brief Serialization to JSON.
         * @tparam U Type of elements.
         * @tparam JCapacity Capacity of vector.
         * @param j JSON object to store result.
         * @param vec Vector to serialize.
         */
        template<typename U, size_t JCapacity>
        friend void from_json(const nlohmann::json &j, SharedVector<U, JCapacity> &vec);

        /**
         * @brief Deserialization from JSON.
         * @tparam U Type of elements.
         * @tparam JCapacity Capacity of vector.
         * @param j JSON array to read from.
         * @param vec Vector to fill.
         * @throw std::runtime_error if JSON is not an array or too large.
         */
        template<typename U, size_t JCapacity>
        friend void to_json(nlohmann::json &j, const SharedVector<U, JCapacity> &vec);

        T data[Capacity];   /**< Storage array for elements */
        size_t size = 0;    /**< Current number of elements */
        size_t capacity = Capacity; /**< Maximum capacity */
};

template<typename T, size_t Capacity>
void SharedVector<T, Capacity>::push_back(const T &item) {
    if (size >= Capacity) throw std::runtime_error("vector reached full capacity");
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

    if (j.size() > Capacity)
        throw std::runtime_error("JSON array too large for SharedVector capacity");

    vec.size = j.size();
    T* data = vec.begin();

    for (size_t i = 0; i < vec.size; ++i) {
        data[i] = j[i].get<T>();
    }
}

#endif //PROJEKT_SHAREDVECTOR_H