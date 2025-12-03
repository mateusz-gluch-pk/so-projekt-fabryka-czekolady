//
// Created by mateusz on 3.12.2025.
//

#ifndef PROJEKT_IRUNNABLE_H
#define PROJEKT_IRUNNABLE_H

class IRunnable {
    public:
        virtual ~IRunnable() = default;
        virtual void run(); // fork
        virtual void stop(); // SIGTERM
        virtual void pause(); // SIGSTOP
        virtual void resume(); // SIGCONT
        virtual void reload(); // SIGHUP
};


#endif //PROJEKT_IRUNNABLE_H