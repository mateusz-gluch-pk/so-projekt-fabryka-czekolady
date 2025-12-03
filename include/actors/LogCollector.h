//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_LOGCOLLECTOR_H
#define PROJEKT_LOGCOLLECTOR_H
#include <iostream>
#include <fstream>

#include "IRunnable.h"
#include "Message.h"
#include "MessageQueue.h"
#include "ipcs/MessageQueue.h"


class LogCollector: public IRunnable{

    public:
        LogCollector(MessageQueue<Message> msq, std::string filename, bool tty = true);
        ~LogCollector();

        void run() override {
            while (true) {
                Message msg;
                _msq->receive(&msg);
                _write_log(msg);
            }
        };

        void stop() override;
        void pause() override;
        void resume() override;
        void reload() override;

    private:
        void _write_log(Message& msg) {
            auto log = msg.string();
            if (_tty) {
                std::cout << log;
            }
            _file << log;
        };

        MessageQueue<Message> *_msq;
        std::string _filename;
        std::ofstream _file;
        bool _tty;
};


#endif //PROJEKT_LOGCOLLECTOR_H