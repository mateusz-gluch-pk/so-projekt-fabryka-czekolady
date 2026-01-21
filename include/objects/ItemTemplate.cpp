//
// Created by mateusz on 26.11.2025.
//

#include "ItemTemplate.h"

ItemTemplate::ItemTemplate(const std::string &name, const int size, const int base_delay) :
    _name(std::move(name)),
    _size(size),
    _base_delay(base_delay) {
}

int ItemTemplate::delay_ms() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution dist(0.0, 1.0);

    const double factor = dist(gen);
    return static_cast<int>(_base_delay * (0.5 + factor));
}

int ItemTemplate::base_delay_ms() const {
    return _base_delay;
}

std::unique_ptr<IItem> ItemTemplate::get() const {
    switch (_size) {
        case 1:
            return std::make_unique<Item<1>>(_name);
        case 2:
            return std::make_unique<Item<2>>(_name);
        case 3:
            return std::make_unique<Item<3>>(_name);
        default:
            return nullptr;
    }
}


