/**
 * @file        actuators/actuatordelegate.hpp
 * @brief       Delegate for actuator components
 * @author      Justin Scott
 * @date        2026-10-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/manager.hpp>

#include <ui/common/layout/delegate.hpp>

namespace ui::actuators
{
//////////////////////////////////////////////////////////////////////////////
/// Delegate for actuator components
///
//////////////////////////////////////////////////////////////////////////////
class ActuatorDelegate : public common::LayoutDelegate
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  automations     List of automations
    ///
    //////////////////////////////////////////////////////////////////////////////
    explicit ActuatorDelegate (control::AutomationManager& automations) :
        m_automations (automations)
        {}

    //////////////////////////////////////////////////////////////////////////////
    /// Open a dialog to create an actuator
    ///
    /// @param[in]  controller      Controller to create actuator for
    /// @param[in]  parent          Parent widget
    ///
    /// @return     Actuator button
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual common::LayoutButton* openCreate (control::ControllerBase&  controller,
                                              QWidget*                  parent) override;

    //////////////////////////////////////////////////////////////////////////////
    /// Create the buttons for the actuator buttons for the group box
    ///
    /// @param[in]  controller      Controller to create buttons for
    /// @param[in]  addToLayout     Callback to add the button to the layout
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void createButtons (control::ControllerBase&                    controller,
                                std::function<void (common::LayoutButton*)> addToLayout) override;

private:
    control::AutomationManager& m_automations;  ///< List of automations

    //////////////////////////////////////////////////////////////////////////////
    /// Create a button for an actuator
    ///
    /// @param[in]  actuator        Actuator to create the button for
    ///
    /// @return     BUtton for actuator
    ///
    //////////////////////////////////////////////////////////////////////////////
    common::LayoutButton* makeButton (const layout::Actuator& actuator);
    };
} // namespace ui::actuators