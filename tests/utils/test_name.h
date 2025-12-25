//
// Created by mateusz on 25.12.2025.
//

#ifndef FACTORY_TEST_NAME_H
#define FACTORY_TEST_NAME_H

#include <cstdlib>
#include <ctime>
#include <sstream>


inline std::string test_name() {
    std::ostringstream oss;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    oss << "test" << std::rand() % 1000000;
    return oss.str();
}
#endif //FACTORY_TEST_NAME_H