//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_LAYOUT_H
#define FACTORY_LAYOUT_H

#include <utility>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"


class Layout {
public:
    Layout(ftxui::Component dashboard, ftxui::Component control_panel, ftxui::Component log_panel);

    ftxui::Component component();

private:
    ftxui::Component _dashboard;
    ftxui::Component _control_panel;
    ftxui::Component _log_panel;

    ftxui::Component _top_row;
    ftxui::Component _root;
};

#endif //FACTORY_LAYOUT_H