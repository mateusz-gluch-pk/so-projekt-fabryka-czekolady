//
// Created by mateusz on 7.12.2025.
//

#ifndef PROJEKT_IQUEUE_H
#define PROJEKT_IQUEUE_H

/**
 * @brief Interface for a generic queue.
 *
 * Provides the basic operations for sending and receiving messages.
 * Can be implemented using System V message queues, in-memory queues, or mocks.
 *
 * @tparam T Type of data stored in the queue.
 */
template<typename T>
class IQueue {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~IQueue() = default;

    /**
     * @brief Send a message to the queue.
     * @param data Message to send.
     * @note Default implementation does nothing.
     */
    virtual void send(T data) const {}

    /**
     * @brief Receive a message from the queue.
     * @param data Pointer to store received message.
     * @note Default implementation does nothing.
     */
    virtual void receive(T *data) const {}
};

#endif //PROJEKT_IQUEUE_H