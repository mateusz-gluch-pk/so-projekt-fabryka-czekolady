//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_ITEM_H
#define PROJEKT_ITEM_H
#include <string>
#include <nlohmann/json.hpp>

#define ITEM_NAME_LENGTH 20

class Item {
    public:
        Item();
        Item(const std::string &name, int size, int count);

        Item& operator=(const Item& other) {
            if (this == &other) return *this;
            std::memcpy(_name, other._name, ITEM_NAME_LENGTH);
            _size  = other._size;
            _count = other._count;
            return *this;
        }

        bool operator==(const Item& other) const {
            return strcmp(_name, other._name) == 0 && _size == other._size;
        }

        // [[nodiscard]] is a modern cpp quirk...
        [[nodiscard]] std::string name() const {return _name;};
        [[nodiscard]] int size() const {return _size;};
        [[nodiscard]] int count() const {return _count;};

        int stack(Item &item);
        Item *unstack();

        friend void from_json(const nlohmann::json &j, Item &item);
        friend void to_json(nlohmann::json &j, const Item &item);

    private:
        char _name[ITEM_NAME_LENGTH];
        int _size;
        int _count;
};

inline void to_json(nlohmann::json &j, const Item &item) {
    j = {
        {"name", item._name},
        {"count", item._count},
        {"size", item._size}
    };
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