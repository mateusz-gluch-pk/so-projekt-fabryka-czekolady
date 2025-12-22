//
// Created by mateusz on 26.11.2025.
//

#include "Recipe.h"

Recipe::Recipe(const std::vector<Item> &inputs, const Item &output): _inputs(inputs), _output(output) {}

std::string Recipe::try_produce(std::vector<Item> &inputs, Item *output) const {
    for (const Item &item : _inputs) {
        if (std::ranges::find(inputs, item) == inputs.end()) {
            return item.name();
        }
    }

    // if output is produced, inputs are consumed
    inputs.clear();
    *output = Item(_output.name(), _output.size(), 1);
    return"";
}
