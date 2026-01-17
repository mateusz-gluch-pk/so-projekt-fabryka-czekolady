//
// Created by mateusz on 17.01.2026.
//

#ifndef FACTORY_WORKERTABLE_H
#define FACTORY_WORKERTABLE_H

#include "ftxui/component/component.hpp"
#include "services/WorkerService.h"

class WorkerTable {
public:
    explicit WorkerTable(WorkerService &svc);

    ftxui::Component component() {return _component;}

private:
    [[nodiscard]] ftxui::Element Render() const;

    bool OnEvent(const ftxui::Event& e);

    WorkerService &_svc;
    int _scroll = 0;
    ftxui::Component _component;
};


#endif //FACTORY_WORKERTABLE_H