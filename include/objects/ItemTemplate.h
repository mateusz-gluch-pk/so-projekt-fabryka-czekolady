//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_ITEMTEMPLATE_H
#define PROJEKT_ITEMTEMPLATE_H
#include <random>
#include <string>

#include "Item.h"


/**
 * @brief Template for creating items with a base size and delay.
 */
class ItemTemplate {
public:
    /**
     * @brief Construct an ItemTemplate.
     * @param name       Name of the item
     * @param size       Size of the item
     * @param base_delay Base delay in milliseconds
     */
    ItemTemplate(const std::string &name, int size, int base_delay);

    /// Default constructor (empty template)
    ItemTemplate() {};

    /**
     * @brief Get the randomized delay of the item.
     * @return Delay in milliseconds
     */
    [[nodiscard]] int delay_ms() const;

    /**
     * @brief Get the base delay of the item.
     * @return Delay in milliseconds
     */
    [[nodiscard]] int base_delay_ms() const;

    /**
     * @brief Create an Item from this template.
     * @return Item object with the template's name and size
     */
    [[nodiscard]] std::unique_ptr<IItem> get() const;

private:
    std::string _name;   ///< Item name
    int _size = 0;       ///< Item size
    int _base_delay = 0; ///< Base delay in milliseconds
};

#endif //PROJEKT_ITEMTEMPLATE_H