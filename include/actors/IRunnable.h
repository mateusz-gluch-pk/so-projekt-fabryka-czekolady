//
// Created by mateusz on 3.12.2025.
//

#ifndef PROJEKT_IRUNNABLE_H
#define PROJEKT_IRUNNABLE_H

class IRunnable {
    public:
        virtual ~IRunnable() = default;
        virtual void run() = 0; // fork
        virtual void stop() = 0; // SIGTERM
        virtual void pause() = 0; // SIGSTOP
        virtual void resume() = 0; // SIGCONT
        virtual void reload() = 0; // SIGHUP
};


#endif //PROJEKT_IRUNNABLE_H