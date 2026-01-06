//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_DASHBOARD_H
#define FACTORY_DASHBOARD_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include "ftxui/component/event.hpp"
#include "services/DelivererService.h"
#include "services/WarehouseService.h"
#include <services/WorkerService.h>

class DelivererTable {
public:
    explicit DelivererTable(DelivererService &svc)
        : _svc(svc)
    {
        _component = ftxui::CatchEvent(
          ftxui::Renderer([&] { return Render(); }),
          [&](const ftxui::Event& e) { return OnEvent(e); }
        );
    }

    ftxui::Component component() {return _component;}

private:
    ftxui::Element Render() {
        ftxui::Elements elements;
        auto workers = _svc.get_all();

        // Header
        ftxui::Elements headers;
        for (auto& h : DelivererStats::headers()) {
            headers.push_back(ftxui::text(" " + h + " ") | ftxui::bold );
        }
        elements.push_back(ftxui::hbox(headers));

        // Rows
        for (int i = _scroll; i < workers.size(); ++i) {
            ftxui::Elements cells;
            for (auto& h : workers[i]->row()) {
                headers.push_back(ftxui::text(" " + h + " ") | ftxui::bold );
            }
            elements.push_back(ftxui::hbox(cells));
        }

        return ftxui::vbox(elements)
               | ftxui::frame
               | ftxui::vscroll_indicator
               | ftxui::border;
    }

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


class WarehouseTable {
public:
    explicit WarehouseTable(WarehouseService &svc)
        : _svc(svc)
    {
        _component = ftxui::CatchEvent(
          ftxui::Renderer([&] { return Render(); }),
          [&](const ftxui::Event& e) { return OnEvent(e); }
        );
    }

    ftxui::Component component() {return _component;}

private:
    ftxui::Element Render() {
        ftxui::Elements elements;
        auto workers = _svc.get_all_stats();

        // Header
        ftxui::Elements headers;
        for (auto& h : WarehouseStats::headers()) {
            headers.push_back(ftxui::text(" " + h + " ") | ftxui::bold );
        }
        elements.push_back(ftxui::hbox(headers));

        // Rows
        for (int i = _scroll; i < workers.size(); ++i) {
            ftxui::Elements cells;
            for (auto& h : workers[i].row()) {
                headers.push_back(ftxui::text(" " + h + " ") | ftxui::bold );
            }
            elements.push_back(ftxui::hbox(cells));
        }

        return ftxui::vbox(elements)
               | ftxui::frame
               | ftxui::vscroll_indicator
               | ftxui::border;
    }

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

    WarehouseService &_svc;
    int _scroll = 0;
    ftxui::Component _component;
};


class WorkerTable {
public:
    explicit WorkerTable(WorkerService &svc)
        : _svc(svc)
    {
        _component = ftxui::CatchEvent(
          ftxui::Renderer([&] { return Render(); }),
          [&](const ftxui::Event& e) { return OnEvent(e); }
        );
    }

    ftxui::Component component() {return _component;}

private:
    ftxui::Element Render() {
        ftxui::Elements elements;
        auto workers = _svc.get_all();

        // Header
        ftxui::Elements headers;
        for (auto& h : WorkerStats::headers()) {
            headers.push_back(ftxui::text(" " + h + " ") | ftxui::bold );
        }
        elements.push_back(ftxui::hbox(headers));

        // Rows
        for (int i = _scroll; i < workers.size(); ++i) {
            ftxui::Elements cells;
            for (auto& h : workers[i]->row()) {
                headers.push_back(ftxui::text(" " + h + " ") | ftxui::bold );
            }
            elements.push_back(ftxui::hbox(cells));
        }

        return ftxui::vbox(elements)
               | ftxui::frame
               | ftxui::vscroll_indicator
               | ftxui::border;
    }

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

    WorkerService &_svc;
    int _scroll = 0;
    ftxui::Component _component;
};

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
            warehouses,
            workers,
            deliverers,
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