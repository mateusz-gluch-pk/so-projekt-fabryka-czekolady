//
// Created by mateusz on 2.01.2026.
//

#ifndef FACTORY_CONTROLPANEL_H
#define FACTORY_CONTROLPANEL_H
#include "Supervisor.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/loop.hpp"


/**
 * @class ControlPanel
 * @brief UI control panel for managing the simulation lifecycle.
 *
 * Provides a vertical set of buttons allowing the user to stop
 * workers, deliverers, warehouses, or the entire simulation.
 *
 * @inputs
 *  - User interactions via FTXUI buttons.
 *
 * @outputs
 *  - Invokes control commands on the associated Supervisor instance.
 */
class ControlPanel {
public:
    /**
     * @brief Constructs the control panel bound to a Supervisor.
     *
     * Initializes all control buttons and binds their callbacks
     * to the corresponding Supervisor control methods.
     *
     * @param sv Reference to the Supervisor controlling the simulation.
     */
    explicit ControlPanel(Supervisor& sv);

    /**
     * @brief Returns the root FTXUI component for rendering.
     *
     * @return ftxui::Component representing the control panel UI.
     */
    [[nodiscard]] ftxui::Component component() const { return _component; }

private:
    /// Reference to the simulation supervisor.
    Supervisor& _supervisor;

    /// Root FTXUI component containing all control buttons.
    ftxui::Component _component;
};

#endif //FACTORY_CONTROLPANEL_H