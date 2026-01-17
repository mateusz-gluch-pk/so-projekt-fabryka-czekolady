//
// Created by mateusz on 17.01.2026.
//

#ifndef FACTORY_DELIVERERTABLE_H
#define FACTORY_DELIVERERTABLE_H

#include "ftxui/component/event.hpp"
#include "ftxui/component/loop.hpp"
#include "services/DelivererService.h"



/**
 * @class DelivererTable
 * @brief Scrollable table view displaying deliverer statistics.
 *
 * This component renders a table of deliverers obtained from the
 * DelivererService and allows vertical scrolling using keyboard events.
 *
 * @inputs
 *  - Deliverer data provided by DelivererService.
 *  - Keyboard events (ArrowUp / ArrowDown).
 *
 * @outputs
 *  - Renders a formatted FTXUI table element.
 */
class DelivererTable {
public:
    /**
     * @brief Constructs the deliverer table bound to a service.
     *
     * @param svc Reference to the DelivererService providing deliverer data.
     */
    explicit DelivererTable(DelivererService& svc);

    /**
     * @brief Returns the root FTXUI component for rendering.
     *
     * @return ftxui::Component representing the deliverer table UI.
     */
    ftxui::Component component() { return _component; }

private:
    /**
     * @brief Renders the deliverer table.
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
    /// Reference to the deliverer service providing data.
    DelivererService& _svc;

    /// Current vertical scroll offset.
    int _scroll = 0;

    /// Root FTXUI component.
    ftxui::Component _component;
};

#endif //FACTORY_DELIVERERTABLE_H