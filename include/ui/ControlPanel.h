//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_CONTROLPANEL_H
#define FACTORY_CONTROLPANEL_H
#include "Supervisor.h"
#include "../../cmake-build-debug/_deps/ftxui-src/include/ftxui/component/component_base.hpp"
#include "../../cmake-build-debug/_deps/ftxui-src/src/ftxui/dom/box_helper.hpp"

class ControlPanel {
    explicit ControlPanel(Supervisor& sv): _supervisor(sv) {}

    ftxui::Component
    private:

    Supervisor& _supervisor;
};


#endif //FACTORY_CONTROLPANEL_H