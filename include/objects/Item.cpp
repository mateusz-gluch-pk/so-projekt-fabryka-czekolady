//
// Created by mateusz on 22.11.2025.
//

#include "Item.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

Item::Item(): _name(""), _size(0), _count(0) {}

Item::Item(const std::string &name, const int size, const int count): _size(size), _count(count) {
    if (name.length() > ITEM_NAME_LENGTH) {
        throw std::out_of_range("Item name is too long");
    }
    strcpy(_name, name.c_str());
}

int Item::stack(Item &item) {
    // names must match to stack
    // size must match too
    if (*this != item) {
        return -1;
    }

    // total count should be preserved -- and stacking is a operation complete
    _count += item._count;
    item._count = 0;
    return _count;
}

Item *Item::unstack() {
    if (_count == 0) {
        return nullptr;
    }

    _count--;
    return new Item(_name, _size, 1);
}

