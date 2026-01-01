//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_RECIPE_H
#define PROJEKT_RECIPE_H
#include <vector>

#include "Item.h"


class Recipe {
    public:
        Recipe(const std::vector<Item> &inputs, const Item &output);
        Recipe() {};
        std::string try_produce(std::vector<Item> &inputs, Item *output) const;
        [[nodiscard]] std::string name() const {return _output.name();};

    private:
        std::vector<Item> _inputs;
        Item _output;
};


#endif //PROJEKT_RECIPE_H