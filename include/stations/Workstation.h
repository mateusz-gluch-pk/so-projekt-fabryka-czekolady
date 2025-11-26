//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_WORKSTATION_H
#define PROJEKT_WORKSTATION_H
#include <vector>
#include <sys/ipc.h>

#include "Item.h"
#include "Recipe.h"
#include "ipcs/Semaphore.h"


class Workstation {
    public:
        Workstation(const std::string &name, Recipe *recipe):
            Workstation(name, recipe, ftok(("workstations/" + name + ".key").c_str(), 1)) {};

        Recipe &recipe() const {return *_recipe;};
        Item &create(std::vector<Item> items);

    private:
        Workstation(const std::string &name, Recipe *recipe, const key_t key): _name(name), _recipe(recipe), _sem(key) {};
        std::string _name;
        Recipe *_recipe;
        Semaphore _sem;
};


#endif //PROJEKT_WORKSTATION_H