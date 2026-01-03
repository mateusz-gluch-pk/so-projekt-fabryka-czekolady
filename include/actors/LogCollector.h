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
#include "ipcs/key.h"
#include "objects/Message.h"
#include "ipcs/MessageQueue.h"
#include "ipcs/SharedMemory.h"
#include "objects/SharedVector.h"

#define LOGGING_DIR "logging"
#define LOGGING_BUFFER_SIZE 1024

class LogCollector: public IRunnable{
    public:
        explicit LogCollector(std::string name, Logger &log, bool tty = true);
        ~LogCollector() override;
        void run(ProcessStats &stats, Logger &log) override;
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;
        const std::string &name() override { return _name; }

    private:
        void _main();
        void _reload() {};
        void _reattach(Logger &log) {
            _file = _open_file();
            _log = log;
            _msq.emplace(make_key(LOGGING_DIR, _name, &log), false);

            size_t bufsize = sizeof(SharedVector<Message, LOGGING_BUFFER_SIZE>) + sizeof(Item) * LOGGING_BUFFER_SIZE;
            _buffer.emplace(make_key(LOGGING_DIR, _name, &log), bufsize, &log, false);
        }

        [[nodiscard]] std::string _msg(const std::string &msg) const {
            return "actors/LogCollector/" + _name + ":\t" + msg;
        }

        std::ofstream _open_file() const;
        void _close_file();

        std::string _name;
        bool _tty;

        std::optional<MessageQueue<Message>> _msq;
        std::optional<SharedMemory<SharedVector<Message, LOGGING_BUFFER_SIZE>>> _buffer;
        Logger &_log;

        std::ofstream _file;
        std::atomic<bool> _running, _paused, _reloading;
};


#endif //PROJEKT_LOGCOLLECTOR_H