//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_WORKSTATION_H
#define PROJEKT_WORKSTATION_H
#include <utility>
#include <vector>
#include <sys/ipc.h>

#include "objects/Item.h"
#include "objects/Recipe.h"
#include "ipcs/Semaphore.h"


class Workstation {
    public:
        Workstation(const std::string &name, Recipe *recipe, Logger *log):
            Workstation(name, recipe, ftok(("workstations/" + name + ".key").c_str(), 1), log) {};

        Recipe &recipe() const {return *_recipe;};
        Item &create(std::vector<Item> items);

    private:
        Workstation(std::string name, Recipe *recipe, const key_t key, Logger *log): _name(std::move(name)), _log(log), _recipe(recipe), _sem(key, log) {};
        std::string _name;
        Logger *_log;
        Recipe *_recipe;
        Semaphore _sem;
};


#endif //PROJEKT_WORKSTATION_H