//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_LOGCOLLECTOR_H
#define PROJEKT_LOGCOLLECTOR_H
#include <atomic>
#include <fstream>
#include <thread>

#include "IRunnable.h"
#include "objects/Message.h"
#include "ipcs/MessageQueue.h"


class LogCollector: public IRunnable{
    public:
        LogCollector(key_t key, const std::string &filename, const bool tty = true);
        ~LogCollector() override;

        void run() override;
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;

    private:
        void _main();
        void _reload();
        void _reattach() {
            _msq = MessageQueue<Message>::attach(_key);
        }

        std::ofstream _open_file() const;
        void _close_file();
        void _write_log(Message& msg);;

        key_t _key;
        MessageQueue<Message> _msq;
        std::string _filename;
        std::ofstream _file;
        bool _tty;

        std::atomic<bool> _running, _paused, _reloading;
};


#endif //PROJEKT_LOGCOLLECTOR_H