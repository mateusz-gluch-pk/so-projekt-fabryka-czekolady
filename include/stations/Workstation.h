//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_WORKSTATION_H
#define PROJEKT_WORKSTATION_H
#include <utility>
#include <vector>
#include <sys/ipc.h>

#include "ipcs/key.h"
#include "objects/Item.h"
#include "objects/Recipe.h"
#include "ipcs/Semaphore.h"
#include "ipcs/SharedMemory.h"

#define WORKSTATION_DIR "workstations"

class Workstation {
     public:
         static Workstation create(const std::string &name, const Recipe &recipe, Logger &log) {
             return Workstation(name, &recipe, log);
         }
         static Workstation attach(const std::string &name, Logger &log) {
            return Workstation(name, nullptr, log);
        }

        Workstation(const std::string &name, const Recipe *recipe, Logger &log):
            _key(make_key(WORKSTATION_DIR, name, log)),
            _owner(recipe != nullptr),
            _name(name),
            _log(log),
            _sem(_key, &log, _owner),
            _shm(_key, sizeof(Recipe), &log, _owner) {
            // copy recipe over to shared memory
            if (_owner) {
                *_shm = *recipe;
            }
        }

        const Recipe &recipe() const {return *_shm;}

    private:
        [[nodiscard]] std::string _msg(const std::string &msg) const {
            return "stations/Workstation/" + _name + ":\t" + msg;
        }
        key_t _key;
        bool _owner;

        std::string _name;
        Logger &_log;

        SharedMemory<Recipe> _shm;
        Semaphore _sem;
};

#endif //PROJEKT_WORKSTATION_H