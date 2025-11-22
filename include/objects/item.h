//
// Created by mateusz on 22.11.2025.
//

#ifndef PROJEKT_ITEM_H
#define PROJEKT_ITEM_H
#include <string>


class Item {
    public:
        Item(const std::string &name, const int size, const int count) {_name=name; _size=size; _count=count;};
        std::string &name() {return _name;};
        int size() const {return _size;};
        int count() const {return _count;};

        int stack(const Item *item);
        Item *unstack();

    private:
        std::string _name;
        int _size;
        int _count;
};

#endif //PROJEKT_ITEM_H