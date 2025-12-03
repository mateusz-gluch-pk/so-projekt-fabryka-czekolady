//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_SUPERVISOR_H
#define PROJEKT_SUPERVISOR_H


class Supervisor {

    public:
    Supervisor();
    ~Supervisor();

    void run();
    void stop();
    void pause();
    void resume();
    void reload();
};


#endif //PROJEKT_SUPERVISOR_H