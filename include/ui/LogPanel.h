//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_LOGS_H
#define FACTORY_LOGS_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include "services/LoggerService.h"

class LogPanel {
public:
    explicit LogPanel(LoggerService& svc)
        : _svc(svc)
    {
        _component = ftxui::Renderer([&] {
            ftxui::Elements elements;

            const auto logs = _svc.logs();
            if (logs != nullptr) {
                for (const auto& log : *logs) {
                    elements.push_back(_format(log));
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

    ftxui::Component component() { return _component; }

private:
    LoggerService& _svc;
    ftxui::Component _component;

    static ftxui::Element _format(const Message& msg) {
        using namespace ftxui;

        Element line = text(msg.string());

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
};


#endif //FACTORY_LOGS_H