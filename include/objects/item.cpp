//
// Created by mateusz on 22.11.2025.
//

#include "item.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

int Item::stack(const Item &item) {
    if (_name == item._name) {
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

