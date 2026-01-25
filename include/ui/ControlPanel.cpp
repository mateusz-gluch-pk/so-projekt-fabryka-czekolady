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

    auto pause_workers = ftxui::Button(
        "Pause Workers",
        [&]{sv.pause_workers();}
    );

    auto pause_deliverers = ftxui::Button(
        "Pause Deliverers",
        [&]{sv.pause_deliverers();}
    );

    auto resume_workers = ftxui::Button(
        "Resume Workers",
        [&]{sv.resume_workers();}
    );

    auto resume_deliverers = ftxui::Button(
        "Resume Deliverers",
        [&]{sv.resume_deliverers();}
    );

    auto stop_warehouses = ftxui::Button(
        "Stop Warehouses",
        [&]{sv.stop_warehouses();}
    );

    auto pause_warehouses = ftxui::Button(
    "Pause Warehouses",
    [&]{sv.pause_warehouses();}
    );

    auto resume_warehouses = ftxui::Button(
        "Resume Warehouses",
        [&]{sv.resume_warehouses();}
    );


    auto stop_all = ftxui::Button(
        "Stop Simulation",
        [&]{sv.stop_all();}
    );


    auto stop_workers_whs = ftxui::Button(
        "Stop Workers+Warehouses",
        [&]{sv.stop_workers_warehouses();}
    );


    auto col1 = ftxui::Container::Vertical({stop_workers, stop_deliverers, stop_warehouses});
    auto col2 = ftxui::Container::Vertical({pause_workers, pause_deliverers, pause_warehouses});
    auto col3 = ftxui::Container::Vertical({resume_workers, resume_deliverers, resume_warehouses});
    auto col4 = ftxui::Container::Vertical({stop_all, stop_workers_whs});
    _component = ftxui::Container::Horizontal({col1, col2, col3, col4});
}
