//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_RECIPE_H
#define PROJEKT_RECIPE_H
#include <vector>

#include "Item.h"


/**
 * @brief Represents an Item crafting recupe
 */
class Recipe {
public:
    /**
     * @brief Construct a Recipe.
     * @param inputs  List of input items required
     * @param output  Output item produced by the recipe
     */
    Recipe(std::vector<std::unique_ptr<IItem>> inputs,
            std::unique_ptr<IItem> output)
         : _inputs(std::move(inputs)), _output(std::move(output)) {}

    /// Default constructor (empty recipe)
    Recipe() {};

    /**
     * @brief Attempt to produce the output from given inputs.
     *
     * Checks if the provided items satisfy the recipe requirements.
     * If successful, the output is stored in `output`.
     *
     * @param inputs  Vector of items to consume
     * @param output  Pointer to store the produced item
     * @return Empty string if successful, otherwise a missing item name
     */
    std::string try_produce(std::vector<std::unique_ptr<IItem>>& inputs,
                            std::unique_ptr<IItem>& output) const;

    /**
     * @brief Get the name of the output item.
     * @return Name of the output item
     */
    [[nodiscard]] std::string name() const { return _output->name(); }

    std::string input_names() const {
        std::string names;
        bool begin = true;
        for (auto &input : _inputs) {
            if (begin) {
                names = input->name();
                begin = false;
            }
            names += "," + input->name();
        }
        return names;
    }

    std::string input_sizes() const {
        std::string sizes;
        bool begin = true;
        for (auto &input : _inputs) {
            if (begin) {
                sizes = std::to_string(input->size());
                begin = false;
            }
            sizes += "," + std::to_string(input->size());
        }
        return sizes;
    }



private:
    std::vector<std::unique_ptr<IItem>> _inputs; ///< Items required as input
    std::unique_ptr<IItem> _output;              ///< Item produced by the recipe
};

#endif //PROJEKT_RECIPE_H