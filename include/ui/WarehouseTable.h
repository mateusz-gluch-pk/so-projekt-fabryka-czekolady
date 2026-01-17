//
// Created by mateusz on 17.01.2026.
//

#ifndef FACTORY_WAREHOUSETABLE_H
#define FACTORY_WAREHOUSETABLE_H
#include "ftxui/component/component.hpp"
#include "services/WarehouseService.h"


class WarehouseTable {
public:
    explicit WarehouseTable(WarehouseService &svc);

    ftxui::Component component() {return _component;}

private:
    [[nodiscard]] ftxui::Element Render() const;

    bool OnEvent(const ftxui::Event& e);

    WarehouseService &_svc;
    int _scroll = 0;
    ftxui::Component _component;
};

#endif //FACTORY_WAREHOUSETABLE_H