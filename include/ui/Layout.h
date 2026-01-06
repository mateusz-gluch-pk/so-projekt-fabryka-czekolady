//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_LAYOUT_H
#define FACTORY_LAYOUT_H

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"


class Layout {
public:
    Layout(ftxui::Component dashboard, ftxui::Component control_panel, ftxui::Component log_panel):
    _dashboard(dashboard), _control_panel(control_panel), _log_panel(log_panel)
    {
        // --- Compose top row (Dashboard | ControlPanel) ---
        _top_row = ftxui::Container::Horizontal({
            _dashboard |
            ftxui::flex, _control_panel
        });

        // --- Compose root layout (top row + logs) ---
        _root = ftxui::Container::Vertical({
            _top_row | ftxui::flex,
            _log_panel | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 7)
        });
    }

    ftxui::Component component() {
        return _root;
    }

private:
    ftxui::Component _dashboard;
    ftxui::Component _control_panel;
    ftxui::Component _log_panel;

    ftxui::Component _top_row;
    ftxui::Component _root;
};

#endif //FACTORY_LAYOUT_H