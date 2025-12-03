//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_DELIVERER_H
#define PROJEKT_DELIVERER_H
#include "IRunnable.h"


class Deliverer : public IRunnable {
    public:
        Deliverer();
        ~Deliverer();

        void run() override;
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;
};


#endif //PROJEKT_DELIVERER_H