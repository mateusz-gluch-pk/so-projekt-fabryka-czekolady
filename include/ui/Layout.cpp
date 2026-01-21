//
// Created by mateusz on 2.01.2026.
//

#include "Layout.h"

Layout::Layout(ftxui::Component dashboard, ftxui::Component control_panel, ftxui::Component log_panel):
    _dashboard(std::move(dashboard)), _control_panel(std::move(control_panel)), _log_panel(std::move(log_panel)) {
    // --- Compose top row (Dashboard | ControlPanel) ---
    _top_row = ftxui::Container::Horizontal({
        _dashboard |
        ftxui::flex, _control_panel
    });

    // --- Compose root layout (top row + logs) ---
    _root = ftxui::Container::Vertical({
        _top_row | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 15),
        _log_panel | ftxui::flex
    });
}

ftxui::Component Layout::component() {
    return _root;
}
