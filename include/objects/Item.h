//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_ITEM_H
#define PROJEKT_ITEM_H
#include <string>
#include <nlohmann/json.hpp>

#define ITEM_NAME_LENGTH 20

/**
 * @brief Represents an item with a name, size, and count.
 */
class Item {
public:
    /// Default constructor (empty item)
    Item();

    /**
     * @brief Construct an item.
     * @param name  Item name
     * @param size  Item size
     * @param count Item quantity
     */
    Item(const std::string &name, int size, int count);

    /**
     * @brief Copy assignment
     * @param other Item to copy from
     * @return Reference to this item
     */
    Item& operator=(const Item& other) {
        if (this == &other) return *this;
        std::memcpy(_name, other._name, ITEM_NAME_LENGTH);
        _size  = other._size;
        _count = other._count;
        return *this;
    }

    /**
     * @brief Check equality
     * @param other Item to compare with
     * @return true if same name and size
     */
    bool operator==(const Item& other) const {
        return strcmp(_name, other._name) == 0 && _size == other._size;
    }

    [[nodiscard]] std::string name() const { return _name; }  ///< Item name
    [[nodiscard]] int size() const { return _size; }          ///< Item size
    [[nodiscard]] int count() const { return _count; }        ///< Item quantity

    /**
     * @brief Stack another item onto this one.
     * @param item Item to stack
     * @return Number of items stacked
     */
    int stack(Item &item);

    /**
     * @brief Remove one item from the stack.
     * @return Pointer to the unstacked Item, nullptr if empty
     */
    Item *unstack();

    /**
     * @brief Deserialize from JSON.
     * @param j JSON object
     * @param item Item to populate
     */
    friend void from_json(const nlohmann::json &j, Item &item);

    /**
     * @brief Serialize to JSON.
     * @param j JSON object
     * @param item Item to serialize
     */
    friend void to_json(nlohmann::json &j, const Item &item);

private:
    char _name[ITEM_NAME_LENGTH]; ///< Null-terminated item name
    int _size;                     ///< Item size
    int _count;                    ///< Item quantity
};

inline void to_json(nlohmann::json &j, const Item &item) {
    j = {{"name", item._name}, {"count", item._count}, {"size", item._size}};
}

inline void from_json(const nlohmann::json &j, Item &item) {
    std::string name;
    j.at("name").get_to(name);
    std::strncpy(item._name, name.c_str(), ITEM_NAME_LENGTH - 1);
    item._name[ITEM_NAME_LENGTH - 1] = '\0';
    j.at("size").get_to(item._size);
    j.at("count").get_to(item._count);
}


#endif //PROJEKT_ITEM_H