//
// Created by mateusz on 25.12.2025.
//

#ifndef FACTORY_KEY_H
#define FACTORY_KEY_H
#include <filesystem>
#include <fstream>
#include <sys/ipc.h>
#include <sys/types.h>

#include "logger/Logger.h"

namespace fs = std::filesystem;

inline key_t make_key(const std::string dir, const std::string& name, const Logger *log = nullptr) {
    fs::create_directories(dir);

    const std::string key_filename = dir + "/" + name + ".key";
    if (!fs::exists(key_filename)) {
        if (log != nullptr) {
            log->debug("%s/key:\tCreating %s", dir.c_str(), key_filename.c_str());
        }
        std::ofstream _stream(key_filename);
    }

    const key_t key = ftok(key_filename.c_str(), 1);
    if (log != nullptr) {
        log->debug("%s/key:\tFetched %s - %x", dir.c_str(), key_filename.c_str(), key);
    }

    return key;
}

#endif //FACTORY_KEY_H