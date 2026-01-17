//
// Created by mateusz on 17.01.2026.
//

#ifndef FACTORY_WAREHOUSETABLE_H
#define FACTORY_WAREHOUSETABLE_H
#include "ftxui/component/component.hpp"
#include "services/WarehouseService.h"


class WarehouseService;

/**
 * @class WarehouseTable
 * @brief Scrollable table view displaying warehouse statistics.
 *
 * Renders a table of warehouses provided by WarehouseService and supports
 * vertical scrolling via keyboard input.
 *
 * @inputs
 *  - Warehouse data supplied by WarehouseService.
 *  - Keyboard events (ArrowUp / ArrowDown).
 *
 * @outputs
 *  - Formatted FTXUI table rendered as a UI component.
 */
class WarehouseTable {
public:
    /**
     * @brief Constructs the warehouse table bound to a service.
     *
     * @param svc Reference to the WarehouseService providing warehouse data.
     */
    explicit WarehouseTable(WarehouseService& svc);

    /**
     * @brief Returns the root FTXUI component for rendering.
     *
     * @return ftxui::Component representing the warehouse table UI.
     */
    ftxui::Component component() { return _component; }

private:
    /**
     * @brief Renders the warehouse table.
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
    /// Reference to the warehouse service providing data.
    WarehouseService& _svc;

    /// Current vertical scroll offset.
    int _scroll = 0;

    /// Root FTXUI component.
    ftxui::Component _component;
};


#endif //FACTORY_WAREHOUSETABLE_H