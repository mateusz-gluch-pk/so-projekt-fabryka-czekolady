//
// Created by mateusz on 2.01.2026.
//

#include "LogPanel.h"

static std::string expand_tabs(const std::string &line, int tab_width = 4) {
    std::string out;
    int col = 0;
    for (const char c : line) {
        if (c == '\t') {
            const int spaces = tab_width - (col % tab_width);
            out.append(spaces, ' ');
            col += spaces;
        } else {
            out.push_back(c);
            col++;
        }
    }
    return out;
}

LogPanel::LogPanel(LoggerService &svc): _svc(svc) {
    _component = ftxui::Renderer([&] {
        ftxui::Elements elements;

        // this is a little workaround - TODO develop a callback to contain enough logs to display in panel
        if (const auto logs = _svc.logs(); logs != nullptr) {
            size_t count = std::min<size_t>(50, logs->size);
            for (size_t i = 0; i < count; ++i) {
                auto& msg = (*logs)[logs->size - 1 - i];
                elements.push_back(_format(msg));
            }
        }

        return ftxui::window(
            ftxui::text(" Logs "),
            ftxui::vbox(std::move(elements))
            | ftxui::frame
            | ftxui::yframe
            | ftxui::focusPositionRelative(0.f, 1.f)
        );
    });
}

ftxui::Element LogPanel::_format(const Message &msg) {
    using namespace ftxui;

    const Element line = text(expand_tabs(msg.string(), 8));

    switch (msg._level) {
        case DEBUG:
            return line | color(Color::GrayLight);
        case INFO:
            return line | color(Color::White);
        case WARNING:
            return line | color(Color::Yellow);
        case ERROR:
            return line | color(Color::Red);
        case FATAL:
            return line | color(Color::RedLight) | bold;
        default:
            return line | color(Color::GrayDark);
    }
}
