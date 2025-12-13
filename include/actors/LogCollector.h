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
        LogCollector(MessageQueue<Message> *msq, const std::string &filename, const bool tty = true);
        ~LogCollector() override;

        void run() override;
        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;

    private:
        void _main();
        void _reload();

        std::ofstream _open_file() const;
        void _close_file();
        void _write_log(Message& msg);;

        MessageQueue<Message> *_msq;
        std::string _filename;
        std::ofstream _file;
        bool _tty;

        std::atomic<bool> _running, _paused, _reloading;
};


#endif //PROJEKT_LOGCOLLECTOR_H