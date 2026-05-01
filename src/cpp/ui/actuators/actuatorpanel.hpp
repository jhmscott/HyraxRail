/**
 * @file        actuators/actuatorpanel.hpp
 * @brief       Panel for the actuator tab of the main window
 * @author      Justin Scott
 * @date        2026-02-01
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

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
//////////////////////////////////////////////////////////////////////////////
class ActuatorPanel : public QWidget
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Actuator configuration tab
    ///
    /// @param[in]  controllers     Controller manager
    /// @param[in]  parent          Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    ActuatorPanel (control::ControllerManager* controllers, QWidget* parent);

private:
    control::ControllerManager* m_controllers;  ///< Controller manager

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
    void add (control::ControllerBase& controller);
    };
} // namespace ui::actuators