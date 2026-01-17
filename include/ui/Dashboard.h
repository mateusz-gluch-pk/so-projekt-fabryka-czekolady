//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_DASHBOARD_H
#define FACTORY_DASHBOARD_H

#include <utility>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "ftxui/component/event.hpp"
#include "ftxui/dom/table.hpp"

/**
 * @class Dashboard
 * @brief Tabbed dashboard UI for monitoring simulation subsystems.
 *
 * This component provides a tab-based interface allowing the user
 * to switch between views for warehouses, workers, and deliverers.
 *
 * @inputs
 *  - Three FTXUI components representing subsystem views.
 *  - User tab selection via the toggle control.
 *
 * @outputs
 *  - Displays the currently selected subsystem component.
 */
class Dashboard {
public:

    /**
     * @brief Constructs the dashboard with subsystem views.
     *
     * Initializes a tab selector and binds each tab to the corresponding
     * subsystem component.
     *
     * @param warehouses Component displaying warehouse state.
     * @param workers Component displaying worker state.
     * @param deliverers Component displaying deliverer state.
     */
    explicit Dashboard(ftxui::Component warehouses, ftxui::Component workers, ftxui::Component deliverers);

    /**
     * @brief Returns the root FTXUI component for rendering.
     *
     * @return ftxui::Component representing the dashboard UI.
     */
    ftxui::Component component();

private:
    /// Index of the currently selected tab.
    int selected_ = 0;

    /// Titles displayed in the tab selector.
    std::vector<std::string> tab_titles_;

    /// UI element used for tab selection.
    ftxui::Component tab_selector_;

    /// Container holding all tab contents.
    ftxui::Component tabs_;

    /// Root dashboard component.
    ftxui::Component root_;
};

#endif //FACTORY_DASHBOARD_H