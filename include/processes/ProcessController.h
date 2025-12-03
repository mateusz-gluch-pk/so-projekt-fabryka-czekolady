//
// Created by mateusz on 3.12.2025.
//

#ifndef PROJEKT_PROCESSCONTROLLER_H
#define PROJEKT_PROCESSCONTROLLER_H
#include <csignal>
#include <memory>

#include "actors/IRunnable.h"


class ProcessController {
    public:
        explicit ProcessController(std::unique_ptr<IRunnable> proc): _proc(std::move(proc)) {};
        ~ProcessController();;
        void run();
        void stop() const;
        void pause() const;
        void resume() const;
        void reload() const;

    private:
        static void _setup_handlers();
        static void _handle_run(int);;
        static void _handle_stop(int);;
        static void _handle_pause(int);;
        static void _handle_resume(int);;
        static void _handle_reload(int);;

        std::unique_ptr<IRunnable> _proc;
        static std::unique_ptr<ProcessController> _cls;
        pid_t _pid = -1;
};

#endif //PROJEKT_PROCESSCONTROLLER_H