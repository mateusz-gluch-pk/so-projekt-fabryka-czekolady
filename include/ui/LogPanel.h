//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_LOGS_H
#define FACTORY_LOGS_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include "services/LoggerService.h"

/**
 * @class LogPanel
 * @brief Scrollable log viewer displaying recent application messages.
 *
 * Renders the most recent log messages provided by LoggerService,
 * color-coded by severity level and automatically scrolled to the bottom.
 *
 * @inputs
 *  - Log messages supplied by LoggerService.
 *
 * @outputs
 *  - Formatted and colorized log entries rendered in an FTXUI window.
 */
class LogPanel {
public:
    /**
     * @brief Constructs the log panel bound to a logger service.
     *
     * @param svc Reference to the LoggerService providing log messages.
     */
    explicit LogPanel(LoggerService& svc);

    /**
     * @brief Returns the root FTXUI component for rendering.
     *
     * @return ftxui::Component representing the log panel UI.
     */
    ftxui::Component component() { return _component; }

private:
    /**
     * @brief Formats a log message into a styled UI element.
     *
     * Applies color and text decoration based on the message severity.
     *
     * @param msg Log message to format.
     * @return ftxui::Element representing the formatted log entry.
     */
    static ftxui::Element _format(const Message& msg);

private:
    /// Reference to the logger service providing log data.
    LoggerService& _svc;

    /// Root FTXUI component.
    ftxui::Component _component;
};


#endif //FACTORY_LOGS_H