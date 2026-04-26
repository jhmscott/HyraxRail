/**
 * @file        routes/includebutton.hpp
 * @brief       Actuator button for use with the EditRouteDialog.
 *              Includes both the toggle for the actuator itself, and
 *              switch to indicate if it should be included in the route
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/actuator.hpp>

#include <ui/actuators/actuatorbutton.hpp>
#include <ui/common/toggleswitch.hpp>

#include <QWidget>

namespace ui::routes
{

//////////////////////////////////////////////////////////////////////////////
/// Button to configure the inclusion of an actuator in a route
///
//////////////////////////////////////////////////////////////////////////////
class ActuatorIncludeButton : public QWidget
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  actuator    Actuator to include/not include
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    ActuatorIncludeButton (const layout::Actuator& actuator, QWidget* parent);

    //////////////////////////////////////////////////////////////////////////////
    /// Check if the user has selected this actuator for inclusion in the route
    ///
    /// @return     true if the actuator should be included
    ///
    //////////////////////////////////////////////////////////////////////////////
    bool isIncluded () const { return m_switch->getSwitch (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the state of the actuator for the route
    ///
    /// @return     Actuator state
    ///
    //////////////////////////////////////////////////////////////////////////////
    bool actuatorState () const { return m_btn->getState (); };

    //////////////////////////////////////////////////////////////////////////////
    /// Get the actuator this button represents
    ///
    /// @return     Actuator
    ///
    //////////////////////////////////////////////////////////////////////////////
    layout::Actuator getActuator () const { return m_actuator; }

    //////////////////////////////////////////////////////////////////////////////
    /// Set whether this is included in the route or not
    ///
    /// @param[in]  included    True if this should be included in the route
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setIncluded (bool included) { m_switch->setSwitch (included, false); }

    //////////////////////////////////////////////////////////////////////////////
    /// Set the state of actuator
    ///
    /// @param[in]  state       Actuator state
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setState (bool state) { m_btn->setState (state); }

signals:
    //////////////////////////////////////////////////////////////////////////////
    /// Signals the inclusion state has changed
    ///
    /// @param[in]  included        Inclusion state
    ///
    //////////////////////////////////////////////////////////////////////////////
    void includeSwitched (bool included);

private:
    layout::Actuator            m_actuator; ///< Actuator being included or not
    common::ToggleSwitch*       m_switch;   ///< Inclusion in route
    actuators::ActuatorButton*  m_btn;      ///< Actuator button
    };
}