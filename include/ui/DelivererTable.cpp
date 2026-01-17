//
// Created by mateusz on 17.01.2026.
//

#include "DelivererTable.h"

#include "ftxui/component/component.hpp"
#include "ftxui/dom/table.hpp"


DelivererTable::DelivererTable(DelivererService &svc): _svc(svc) {
    _component = ftxui::CatchEvent(
        ftxui::Renderer([&] { return Render(); }),
        [&](const ftxui::Event& e) { return OnEvent(e); }
    );
}

ftxui::Element DelivererTable::Render() const {
    const auto workers = _svc.get_all();

    // Header
    std::vector<std::vector<std::string>> contents;
    std::vector<std::string> headers;
    for (auto& h : DelivererStats::headers()) {
        headers.push_back(h);
    }
    contents.push_back(headers);

    // Rows
    for (int i = _scroll; i < workers.size(); ++i) {
        std::vector<std::string> cells;
        for (auto& h : workers[i]->row()) {
            cells.push_back(h);
        }
        contents.push_back(cells);
    }

    auto table =  ftxui::Table(contents);
    table.SelectColumn(0).Border(ftxui::LIGHT);

    // Make first row bold with a double border.
    table.SelectRow(0).Decorate(ftxui::bold);
    table.SelectRow(0).SeparatorVertical(ftxui::LIGHT);
    table.SelectRow(0).Border(ftxui::DOUBLE);

    return table.Render();
}

bool DelivererTable::OnEvent(const ftxui::Event &e) {
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
