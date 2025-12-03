//
// Created by mateusz on 26.11.2025.
//

#ifndef PROJEKT_LOGCOLLECTOR_H
#define PROJEKT_LOGCOLLECTOR_H
#include "IRunnable.h"
#include "MessageQueue.h"


class LogCollector: public IRunnable{

    public:
    LogCollector();
    ~LogCollector();

    void run() override;
    void stop() override;
    void pause() override;
    void resume() override;
    void reload() override;

    private:
    MessageQueue *_msq;
};


#endif //PROJEKT_LOGCOLLECTOR_H