//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_RECIPE_H
#define PROJEKT_RECIPE_H
#include <vector>

#include "Item.h"


class Recipe {
    public:
        Recipe(const std::vector<Item> &inputs, const Item &output): _inputs(inputs), _output(output) {};

        std::string first_missin

        Item transform(std::vector<Item> inputs) {
            return Item(_output.name(), _output.size(), 1);
        }

    private:
        std::vector<Item> _inputs;
        Item _output;
};


#endif //PROJEKT_RECIPE_H