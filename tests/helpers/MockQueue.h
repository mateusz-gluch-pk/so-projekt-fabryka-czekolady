//
// Created by mateusz on 13.12.2025.
//

#ifndef PROJEKT_MOCKQUEUE_H
#define PROJEKT_MOCKQUEUE_H
#include <iostream>
#include <ostream>

#include "logger/IQueue.h"

template<typename T>
class MockQueue: public IQueue<T> {
    void send(T m) const override {std::cout << m << std::endl;}
    void receive(T *m) const override {}
};

#endif //PROJEKT_MOCKQUEUE_H