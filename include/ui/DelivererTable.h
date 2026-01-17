//
// Created by mateusz on 17.01.2026.
//

#ifndef FACTORY_DELIVERERTABLE_H
#define FACTORY_DELIVERERTABLE_H

#include "ftxui/component/event.hpp"
#include "ftxui/component/loop.hpp"
#include "services/DelivererService.h"


class DelivererTable {
public:
    explicit DelivererTable(DelivererService &svc);

    ftxui::Component component() {return _component;}

private:
    [[nodiscard]] ftxui::Element Render() const;

    bool OnEvent(const ftxui::Event& e) {
        if (e == ftxui::Event::ArrowDown) {
            _scroll++;
            return true;
        }
        if (e == ftxui::Event::ArrowUp) {
            _scroll = std::max(0, _scroll - 1);
            return true;
        }
        return false;
    }

    DelivererService &_svc;
    int _scroll = 0;
    ftxui::Component _component;
};

#endif //FACTORY_DELIVERERTABLE_H