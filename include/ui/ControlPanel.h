//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_CONTROLPANEL_H
#define FACTORY_CONTROLPANEL_H
#include "Supervisor.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"


class ControlPanel {
public:
    explicit ControlPanel(Supervisor& sv);
    ftxui::Component component() {return _component;};

private:
    Supervisor& _supervisor;
    ftxui::Component _component;
};


#endif //FACTORY_CONTROLPANEL_H