//
// Created by mateusz on 25.12.2025.
//

#ifndef FACTORY_TEST_NAME_H
#define FACTORY_TEST_NAME_H

#include <cstdlib>
#include <ctime>
#include <random>
#include <sstream>

inline std::string test_name() {
    static std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist(0, 999999);

    return "test" + std::to_string(dist(rng));
}

#endif //FACTORY_TEST_NAME_H