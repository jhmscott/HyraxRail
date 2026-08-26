/**
 * @file        actuators/actuatorpanel.hpp
 * @brief       Panel for the actuator tab of the main window
 * @author      Justin Scott
 * @date        2026-02-01
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/manager.hpp>
#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/actuators/actuatorbutton.hpp>

#include <ui/common/autogrid.hpp>


#include <QGroupBox>
#include <QWidget>

namespace ui::actuators
{


//////////////////////////////////////////////////////////////////////////////
/// Actautor configuration panel
///
/// @ingroup    APP_PANEL
///
//////////////////////////////////////////////////////////////////////////////
class ActuatorPanel : public QWidget
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Actuator configuration tab
    ///
    /// @param[in]  controllers     Controller manager
    /// @param[in]  automations     List of automations
    /// @param[in]  parent          Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    ActuatorPanel (control::ControllerManager&  controllers,
                   control::AutomationManager&  automations,
                   QWidget*                     parent);

private:
    control::ControllerManager& m_controllers;  ///< Controller manager
    control::AutomationManager& m_automations;  ///< List of automations

private slots:

    //////////////////////////////////////////////////////////////////////////////
    /// Handle a controller being removed
    ///
    /// @param[in]  controller      Controller being removed
    ///
    //////////////////////////////////////////////////////////////////////////////
    void remove (const control::ControllerBase& controller);

    //////////////////////////////////////////////////////////////////////////////
    /// Handle a controller being added by a user. Adds a new group box for it's
    /// actuators
    ///
    /// @param[in]  controller      Controller being added
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addController (control::ControllerBase& controller);
    };
} // namespace ui::actuators