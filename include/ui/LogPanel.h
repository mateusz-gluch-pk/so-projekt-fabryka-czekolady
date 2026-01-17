//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_LOGS_H
#define FACTORY_LOGS_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include "services/LoggerService.h"

class LogPanel {
public:
    explicit LogPanel(LoggerService& svc);

    ftxui::Component component() { return _component; }

private:
    LoggerService& _svc;
    ftxui::Component _component;

    static ftxui::Element _format(const Message& msg);
};



#endif //FACTORY_LOGS_H