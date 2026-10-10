/**
 * @file        routes/routedelegate.hpp
 * @brief       Delegate for route components
 * @author      Justin Scott
 * @date        2026-10-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/manager.hpp>

#include <ui/common/layout/delegate.hpp>

namespace ui::routes
{
//////////////////////////////////////////////////////////////////////////////
/// Delegate for route components
///
//////////////////////////////////////////////////////////////////////////////
class RouteDelegate : public common::LayoutDelegate
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  automations     List of automations
    ///
    //////////////////////////////////////////////////////////////////////////////
    explicit RouteDelegate (control::AutomationManager& automations):
        m_automations (automations)
        {}

    //////////////////////////////////////////////////////////////////////////////
    /// Open a dialog to create a route
    ///
    /// @param[in]  controller      Controller to create component for
    /// @param[in]  parent          Parent widget
    ///
    /// @return     Widget for newly created component, or NULL if none created
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual common::LayoutButton* openCreate (control::ControllerBase&  controller, 
                                              QWidget*                  parent) override;

    //////////////////////////////////////////////////////////////////////////////
    /// Create the buttons for the route buttons for the group box
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
    /// Create a button for a route
    ///
    /// @param[in]  actuator        Actuator to create the button for
    ///
    /// @return     BUtton for actuator
    ///
    //////////////////////////////////////////////////////////////////////////////
    common::LayoutButton* makeButton (control::ControllerBase&  controller, 
                                      const layout::Route&      route);
    };

} // namespace ui::routes