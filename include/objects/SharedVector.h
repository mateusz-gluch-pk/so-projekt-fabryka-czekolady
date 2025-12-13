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

        void push_back(const T &item) {
            if (_size >= _capacity) throw std::runtime_error("full");
            (*this)[_size++] = item;
        }

        void erase(T* it) {
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

        friend void from_json(const nlohmann::json &j, SharedVector<T> &vec);
        friend void to_json(nlohmann::json &j, const SharedVector<T> &vec);
};

template<typename T>
void to_json(nlohmann::json& j, const SharedVector<T>& vec) {
    j = nlohmann::json::array();
    const T* data = vec.begin();
    for (size_t i = 0; i < vec._size; ++i) {
        j.push_back(data[i]);
    }
}

template<typename T>
void from_json(const nlohmann::json& j, SharedVector<T>& vec) {
    if (!j.is_array())
        throw std::runtime_error("JSON is not an array for SharedVector");

    if (j.size() > vec._capacity)
        throw std::runtime_error("JSON array too large for SharedVector capacity");

    vec._size = j.size();
    T* data = vec.begin();

    for (size_t i = 0; i < vec._size; ++i) {
        data[i] = j[i].get<T>();
    }
}

namespace nlohmann {

    template <>
    struct adl_serializer<SharedVector<Item>> {
        static void to_json(json& j, const SharedVector<Item>& vec) {
            j = json::array();
            for (size_t i = 0; i < vec._size; ++i) {
                j.push_back(vec[i]);
            }
        }

        static void from_json(const json& j, SharedVector<Item>& vec) {
            if (!j.is_array())
                throw std::runtime_error("JSON must be array for SharedVector<Item>");
            vec._size = j.size();
            for (size_t i = 0; i < j.size(); ++i) {
                vec[i] = j[i].get<Item>();
            }
        }
    };

} // namespace nlohmann

#endif //PROJEKT_SHAREDVECTOR_H