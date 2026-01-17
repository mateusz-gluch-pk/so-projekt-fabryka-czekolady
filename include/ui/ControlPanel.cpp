//
// Created by mateusz on 2.01.2026.
//

#include "ControlPanel.h"

ControlPanel::ControlPanel(Supervisor &sv): _supervisor(sv) {
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
