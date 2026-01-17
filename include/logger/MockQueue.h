//
// Created by mateusz on 13.12.2025.
//

#ifndef PROJEKT_MOCKQUEUE_H
#define PROJEKT_MOCKQUEUE_H
#include <iostream>
#include <ostream>

#include "logger/IQueue.h"

/**
 * @brief Mock implementation of IQueue for testing or logging.
 *
 * Prints messages to `std::cout` instead of sending them over a real queue.
 * Can be used for debugging or unit tests.
 *
 * @tparam T Type of message stored in the queue.
 */
template<typename T>
class MockQueue : public IQueue<T> {
public:
    /**
     * @brief Send a message (mocked).
     * @param m Message to send.
     * @note Prints the message to `std::cout`.
     */
    void send(T m) const override { std::cout << m << std::endl; }

    /**
     * @brief Receive a message (mocked).
     * @param m Pointer to store received message.
     * @note Does nothing in the mock.
     */
    void receive(T *m) const override {}
};

#endif //PROJEKT_MOCKQUEUE_H