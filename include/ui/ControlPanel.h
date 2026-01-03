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
    explicit ControlPanel(Supervisor& sv): _supervisor(sv) {
        auto stop_workers = ftxui::Button(
            "Stop Workers",
            [&]{sv.stop_workers();}
            );

        auto stop_deliverers = ftxui::Button(
            "Stop Deliverers",
            [&]{sv.stop_deliverers();}
            );

        auto stop_warehouses = ftxui::Button(
            "Stop Warehouses",
            [&]{sv.stop_warehouses();}
            );

        auto stop_all = ftxui::Button(
            "Stop Simulation",
            [&]{sv.stop_all();}
            );

        _component = ftxui::Container::Vertical({stop_workers, stop_deliverers, stop_warehouses, stop_all});
    }

    ftxui::Component component() {return _component;};

private:
    Supervisor& _supervisor;
    ftxui::Component _component;
};


#endif //FACTORY_CONTROLPANEL_H