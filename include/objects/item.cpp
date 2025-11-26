//
// Created by mateusz on 22.11.2025.
//

#include "item.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

Item::Item(const std::string &name, const int size, const int count): _size(size), _count(count) {
    if (name.length() > ITEM_NAME_LENGTH) {
        throw std::out_of_range("Item name is too long");
    }
    strcpy(_name, name.c_str());
}

int Item::stack(const Item &item) {
    if (strcmp(_name, item._name) == 0) {
        return -1;
    }

    if (_size != item._size) {
        return -1;
    }

    _count++;
    return _count;
}

Item *Item::unstack() {
    if (_count == 0) {
        return nullptr;
    }

    _count--;
    return new Item(_name, _size, 1);
}

