//
// Created by mateusz on 7.12.2025.
//

#ifndef PROJEKT_IQUEUE_H
#define PROJEKT_IQUEUE_H

template<typename T>
class IQueue {
    public:
        virtual ~IQueue() = default;
        virtual void send(T data) const {}
        virtual void receive(T *data) const {}
};

#endif //PROJEKT_IQUEUE_H