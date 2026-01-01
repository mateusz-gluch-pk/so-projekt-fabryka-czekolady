//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_ITEMTEMPLATE_H
#define PROJEKT_ITEMTEMPLATE_H
#include <random>
#include <string>

#include "Item.h"


class ItemTemplate {
    public:
        ItemTemplate(const std::string &name, int size, int base_delay);
        ItemTemplate() {};
        [[nodiscard]] int delay_ms() const;
        [[nodiscard]] Item get() const;

    private:
        std::string _name;
        int _size = 0;
        int _base_delay = 0;
};

#endif //PROJEKT_ITEMTEMPLATE_H