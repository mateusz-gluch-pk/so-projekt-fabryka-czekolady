//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_ITEM_H
#define PROJEKT_ITEM_H
#include <string>
#include <nlohmann/json.hpp>

#include "IItem.h"

#define BASE_LEN 16
#define TYPESIZE 8

using json = nlohmann::json;

/**
 * @brief Represents an item with a name, size, and count.
 */
template<int Size>
class Item: public IItem {
public:
    /// Default constructor (empty item)
    Item();

    /**
     * @brief Construct an item.
     * @param name  Item name
     */
    explicit Item(const std::string &name);

    /**
     * @brief Copy assignment
     * @param other Item to copy from
     * @return Reference to this item
     */
    Item& operator=(const Item& other);

    [[nodiscard]] std::string name() const override { return _name; }  ///< Item name
    [[nodiscard]] int size() const override { return Size; }          ///< Item size

    /**
     * @brief Deserialize from JSON.
     * @param j JSON object
     * @param item Item to populate
     */
    template<int Size_>
    friend void from_json(const nlohmann::json &j, Item<Size_> &item);

    /**
     * @brief Serialize to JSON.
     * @param j JSON object
     * @param item Item to serialize
     */
    template<int Size_>
    friend void to_json(nlohmann::json &j, const Item<Size_> &item);

    std::unique_ptr<IItem> clone() const override {
        return std::make_unique<Item>(*this);
    }

private:
    char _name[Size*BASE_LEN-TYPESIZE];      ///< Null-terminated item name
};

template<int Size>
void to_json(nlohmann::json &j, const Item<Size> &item) {
    j = {{"name", item._name}, {"size", Size}};
}

template<int Size>
void from_json(const nlohmann::json &j, Item<Size> &item) {
    std::string name;
    j.at("name").get_to(name);

    constexpr size_t max_len = Size * BASE_LEN - 1 - TYPESIZE; // maximum characters
    size_t copy_len = std::min(name.size(), max_len);

    std::copy_n(name.c_str(), copy_len, item._name);
    item._name[copy_len] = '\0';
}

template<int Size>
Item<Size>::Item() {
    for (size_t i = 0; i < Size * BASE_LEN - TYPESIZE; ++i) {
        _name[i] = '\0';
    }
}

template<int Size>
Item<Size>::Item(const std::string &name) {
    size_t copy_len = std::min(name.length(), static_cast<size_t>(Size * BASE_LEN - 1));
    for (size_t i = 0; i < copy_len; ++i) {
        _name[i] = name[i];
    }
    _name[copy_len] = '\0';
}

template<int Size>
Item<Size> &Item<Size>::operator=(const Item &other) {
    if (this == &other) return *this;
    std::memcpy(_name, other._name, Size*BASE_LEN);
    return *this;
}

// IItem factory
inline std::unique_ptr<IItem> new_item(const std::string name, const int size) {
    switch (size) {
        case 1:
            return std::make_unique<Item<1>>(name);
        case 2:
            return std::make_unique<Item<2>>(name);
        case 3:
            return std::make_unique<Item<3>>(name);
        default:
            throw std::runtime_error("Unknown item size: " + std::to_string(size));
    }
}


#endif //PROJEKT_ITEM_H