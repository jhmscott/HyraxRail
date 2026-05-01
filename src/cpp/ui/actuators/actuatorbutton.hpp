/**
 * @file        actuators/actuatorbutton.hpp
 * @brief       Layout switching device toggle button
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/icontoggle.hpp>

#include <layout/actuator.hpp>

#include <QLabel>

namespace ui::actuators
{

//////////////////////////////////////////////////////////////////////////////
/// Button to set the state of an actuator
///
//////////////////////////////////////////////////////////////////////////////
class ActuatorButton : public QWidget
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  actuator    Actuator this button controls
    /// @param[in]  parent      Parent widget
    /// @param[in]  dummy       (optional) If true, this won't update the actuator
    ///                         state on the controller, it will only toggle locally
    ///
    //////////////////////////////////////////////////////////////////////////////
    ActuatorButton (const layout::Actuator& actuator, QWidget* parent, bool dummy = false);

    //////////////////////////////////////////////////////////////////////////////
    /// Get the state of the button
    ///
    //////////////////////////////////////////////////////////////////////////////
    bool getState () const { return m_button->isChecked (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Set the sate of the button
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setState (bool state);

signals:
    void actuatorDeleted ();

private:
    layout::Actuator    m_actuator; ///< Actuator under control
    QLabel*             m_name;     ///< Actuator name label
    common::IconToggle* m_button;   ///< Toggle button controlling actuator
    bool                m_dummy;    ///< True if this does not control the actuator

private slots:

    //////////////////////////////////////////////////////////////////////////////
    /// Handle the user toggling this button
    ///
    /// @param[in]  state       New toggle state
    ///
    //////////////////////////////////////////////////////////////////////////////
    void onToggle (bool state);

    //////////////////////////////////////////////////////////////////////////////
    /// Handle a request to remove this actuator
    ///
    //////////////////////////////////////////////////////////////////////////////
    void removeActuator ();

    //////////////////////////////////////////////////////////////////////////////
    /// Handle a request to edit this actuator
    ///
    //////////////////////////////////////////////////////////////////////////////
    void editActuator ();
    };
} // namespace ui::actuators