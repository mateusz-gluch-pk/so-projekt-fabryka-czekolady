//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_LOGCOLLECTOR_H
#define PROJEKT_LOGCOLLECTOR_H
#include <atomic>
#include <fstream>
#include <optional>
#include <thread>

#include "IRunnable.h"
#include "objects/Message.h"
#include "ipcs/MessageQueue.h"

#define LOGGING_DIR "logging"

class LogCollector: public IRunnable{
    public:
        LogCollector(std::string name, MessageLevel level = INFO, bool tty = true);
        ~LogCollector();
        void run(ProcessStats &stats, Logger &log) override;
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;
        const std::string &name() override { return _name; }

        Logger &logger() {
            return _log;
        }

    private:
        void _main();
        void _reload() {};
        void _reattach(Logger &log) {
            _log = log;
            _msq.emplace(log.key(), false);
        }

        [[nodiscard]] std::string _msg(const std::string &msg) const {
            return "actors/LogCollector/" + _name + ":\t" + msg;
        }

        std::ofstream _open_file() const;
        void _close_file();
        void _write_log(Message& msg);;

        std::string _name;
        Logger _log;

        std::optional<MessageQueue<Message>> _msq;

        std::string _filename;
        std::ofstream _file;
        bool _tty;

        std::atomic<bool> _running, _paused, _reloading;
};


#endif //PROJEKT_LOGCOLLECTOR_H