//
// Created by mateusz on 2.01.2026.
//

#include "Dashboard.h"

ftxui::Component Dashboard::component() {
    return root_;
}

Dashboard::Dashboard(ftxui::Component warehouses, ftxui::Component workers, ftxui::Component deliverers)
    : tab_titles_({"Warehouses","Workers","Deliverers",})
{
    tab_selector_ = ftxui::Toggle(&tab_titles_, &selected_)
        | ftxui::border
        | ftxui::bgcolor(ftxui::Color::GrayDark);

    tabs_ = ftxui::Container::Tab(
      {std::move(warehouses), std::move(workers), std::move(deliverers)},
      &selected_);

    root_ = ftxui::Container::Vertical({tab_selector_, tabs_});
}