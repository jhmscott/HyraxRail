/**
 * @file        actuators/actuatorgroupbox.hpp
 * @brief       Actuator groupbox, one per controller
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/controllers/base.hpp>

#include <ui/common/autogrid.hpp>

#include <QGroupBox>

namespace ui::actuators
{

//////////////////////////////////////////////////////////////////////////////
/// Group box containing all the actuators for a given controller
///
//////////////////////////////////////////////////////////////////////////////
class ActuatorGroup : public QGroupBox
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller      Controller to list actuators for
    /// @param[in]  parent          Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    ActuatorGroup (control::ControllerBase& controller, QWidget* parent);

private:
    control::ControllerBase*    m_controller;   ///< Controller this refers to
    common::AutoGridLayout*     m_gridLayout;   ///< Grid layout containing buttons for each actuator

    //////////////////////////////////////////////////////////////////////////////
    /// Add an actuator button to the grid
    ///
    /// @param[in]  actuator    Actuator to add
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addActuatorToGrid (const layout::Actuator& actuator);

private slots:

    //////////////////////////////////////////////////////////////////////////////
    /// Handles a new actuator being added to the controller
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addActuator ();

    //////////////////////////////////////////////////////////////////////////////
    /// Handles a new actuator being removed from the controller
    ///
    //////////////////////////////////////////////////////////////////////////////
    void removeActuator ();
    };

} // namespace ui::actuators