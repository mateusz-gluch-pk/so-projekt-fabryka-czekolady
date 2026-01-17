//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_LAYOUT_H
#define FACTORY_LAYOUT_H

#include <utility>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"

/**
 * @class Layout
 * @brief Main application layout compositor.
 *
 * Arranges the dashboard, control panel, and log panel into a
 * vertical layout with a fixed-height top row and flexible log area.
 *
 * @inputs
 *  - Three FTXUI components: dashboard, control panel, and log panel.
 *
 * @outputs
 *  - A single composed FTXUI component representing the full UI layout.
 */
class Layout {
public:
    /**
     * @brief Constructs the application layout.
     *
     * @param dashboard Component displaying the main dashboard.
     * @param control_panel Component providing simulation controls.
     * @param log_panel Component displaying logs and events.
     */
    Layout(ftxui::Component dashboard,
           ftxui::Component control_panel,
           ftxui::Component log_panel);

    /**
     * @brief Returns the root FTXUI component for rendering.
     *
     * @return ftxui::Component representing the complete UI layout.
     */
    ftxui::Component component();

private:
    /// Dashboard component (left side of the top row).
    ftxui::Component _dashboard;

    /// Control panel component (right side of the top row).
    ftxui::Component _control_panel;

    /// Log panel component (bottom section).
    ftxui::Component _log_panel;

    /// Horizontal container holding dashboard and control panel.
    ftxui::Component _top_row;

    /// Root container for the entire layout.
    ftxui::Component _root;
};

#endif //FACTORY_LAYOUT_H