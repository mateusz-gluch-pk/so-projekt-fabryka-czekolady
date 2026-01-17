//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_DASHBOARD_H
#define FACTORY_DASHBOARD_H

#include <utility>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "ftxui/component/event.hpp"
#include "ftxui/dom/table.hpp"


class Dashboard {
public:
    Dashboard(ftxui::Component warehouses,
              ftxui::Component workers,
              ftxui::Component deliverers)
        : tab_titles_({
            "Warehouses",
            "Workers",
            "Deliverers",
          })
    {
        tab_selector_ =   ftxui::Toggle(&tab_titles_, &selected_)
            | ftxui::border
            | ftxui::bgcolor(ftxui::Color::GrayDark);

        tabs_ = ftxui::Container::Tab(
          {
            std::move(warehouses),
            std::move(workers),
            std::move(deliverers),
          },
          &selected_);

        root_ = ftxui::Container::Vertical({
          tab_selector_,
          tabs_,
        });
    }

    ftxui::Component component() {
        return root_;
    }

private:
    int selected_ = 0;

    std::vector<std::string> tab_titles_;
    ftxui::Component tab_selector_;
    ftxui::Component tabs_;
    ftxui::Component root_;
};

#endif //FACTORY_DASHBOARD_H