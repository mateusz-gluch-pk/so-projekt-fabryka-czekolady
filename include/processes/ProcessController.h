//
// Created by mateusz on 3.12.2025.
//

#ifndef PROJEKT_PROCESSCONTROLLER_H
#define PROJEKT_PROCESSCONTROLLER_H
#include <csignal>
#include <memory>
#include <optional>

#include "ProcessStats.h"
#include "actors/IRunnable.h"
#include "ipcs/MessageQueue.h"
#include "ipcs/SharedMemory.h"

class ProcessController {
    public:
        explicit ProcessController(std::unique_ptr<IRunnable> proc, const Logger &log, bool create = true, bool debug = false);

        ~ProcessController();
        void run();
        void stop();
        void pause() const;
        void resume() const;
        void reload() const;
        [[nodiscard]] const ProcessStats &stats() const { return *_stats; };

    private:
        static void _setup_handlers();
        // static void _handle_run(int);
        static void _handle_stop(int);
        static void _handle_pause(int);
        static void _handle_resume(int);
        static void _handle_reload(int);

        key_t _key;
        MessageQueue<Message> *_msq = nullptr;
        Logger _log;

        std::unique_ptr<IRunnable> _proc;
        SharedMemory<ProcessStats> _stats;
        static std::unique_ptr<ProcessController> _cls;

        pid_t _pid;
};

#endif //PROJEKT_PROCESSCONTROLLER_H