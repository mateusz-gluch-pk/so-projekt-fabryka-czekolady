//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_ITEM_H
#define PROJEKT_ITEM_H
#include <string>
#include <nlohmann/json.hpp>


class Item {
    public:
        Item(const std::string &name, const int size, const int count) {_name=name; _size=size; _count=count;};

        // [[nodiscard]] is a modern cpp quirk...
        // ... as well as messy double const
        [[nodiscard]] const std::string &name() const {return _name;};
        [[nodiscard]] int size() const {return _size;};
        [[nodiscard]] int count() const {return _count;};

        int stack(const Item *item);
        Item *unstack();

        friend void from_json(const nlohmann::json &j, Item &item);
        friend void to_json(nlohmann::json &j, const Item &item);

    private:
        std::string _name;
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
    j.at("name").get_to(item._name);
    j.at("size").get_to(item._size);
    j.at("count").get_to(item._count);
}

#endif //PROJEKT_ITEM_H