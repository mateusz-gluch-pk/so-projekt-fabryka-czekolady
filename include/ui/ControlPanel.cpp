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

    auto stop_all = ftxui::Button(
        "Stop Simulation",
        [&]{sv.stop_all();}
    );

    auto row1 = ftxui::Container::Horizontal({
        stop_workers,
        pause_workers,
        resume_workers,
    });
    auto row2 = ftxui::Container::Horizontal({
        stop_deliverers,
        pause_deliverers,
        resume_deliverers,
    });
    auto row3 = ftxui::Container::Horizontal({
        stop_warehouses, stop_all
    });

    _component = ftxui::Container::Vertical({row1, row2, row3});
}
