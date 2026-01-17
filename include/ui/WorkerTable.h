//
// Created by mateusz on 17.01.2026.
//

#ifndef FACTORY_WORKERTABLE_H
#define FACTORY_WORKERTABLE_H

#include "ftxui/component/component.hpp"
#include "services/WorkerService.h"

/**
 * @class WorkerTable
 * @brief Scrollable table view displaying worker statistics.
 *
 * Renders a table of workers provided by WorkerService and supports
 * vertical scrolling via keyboard input.
 *
 * @inputs
 *  - Worker data supplied by WorkerService.
 *  - Keyboard events (ArrowUp / ArrowDown).
 *
 * @outputs
 *  - Formatted FTXUI table rendered as a UI component.
 */
class WorkerTable {
public:
    /**
     * @brief Constructs the worker table bound to a service.
     *
     * @param svc Reference to the WorkerService providing worker data.
     */
    explicit WorkerTable(WorkerService& svc);

    /**
     * @brief Returns the root FTXUI component for rendering.
     *
     * @return ftxui::Component representing the worker table UI.
     */
    ftxui::Component component() { return _component; }

private:
    /**
     * @brief Renders the worker table.
     *
     * Builds the table header and visible rows based on the current
     * scroll offset.
     *
     * @return ftxui::Element representing the rendered table.
     */
    [[nodiscard]] ftxui::Element Render() const;

    /**
     * @brief Handles keyboard events for scrolling.
     *
     * @param e Incoming FTXUI event.
     * @return true if the event was handled, false otherwise.
     */
    bool OnEvent(const ftxui::Event& e);

private:
    /// Reference to the worker service providing data.
    WorkerService& _svc;

    /// Current vertical scroll offset.
    int _scroll = 0;

    /// Root FTXUI component.
    ftxui::Component _component;
};


#endif //FACTORY_WORKERTABLE_H