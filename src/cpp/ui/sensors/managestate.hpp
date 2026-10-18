/**
 * @file        sensors/managestate.hpp
 * @brief       Widget to display and manage the state of a sensor port
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/gesture.hpp>

#include <ui/sensors/sensorstate.hpp>

namespace ui::sensors
{
//////////////////////////////////////////////////////////////////////////////
/// Widget to display and manage the state of a sensor port
///
//////////////////////////////////////////////////////////////////////////////
class SensorManageState : public common::HoldToOpenMenu<SensorDisplayState>
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  module      Module this sensor is part of
    /// @param[in]  idx         Index of sensor port within module
    /// @param[in]  radius      Radius of the indicator in pixels
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    SensorManageState (const layout::SensorModule& module,
                       size_t                      idx,
                       uint                        radius,
                       QWidget*                    parent);

    //////////////////////////////////////////////////////////////////////////////
    /// Set the display name for this port
    ///
    /// @param[in]  name        Sensor name
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setName (const QString& name) { m_name = name; }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the display name for this port
    ///
    /// @return     sensor name
    ///
    //////////////////////////////////////////////////////////////////////////////
    QString getName () const { return hasCustomName () ? m_name : getPortName (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Check if this sensor has a custom name
    ///
    /// @return     true if this has been given a custom name
    ///
    //////////////////////////////////////////////////////////////////////////////
    bool hasCustomName () const { return not m_name.isEmpty (); }

private:
    QString m_name; ///< Custom name

    //////////////////////////////////////////////////////////////////////////////
    /// Get the default name of the port
    ///
    /// @return     Default port name
    ///
    //////////////////////////////////////////////////////////////////////////////
    QString getPortName () const { return tr ("Port %1").arg (getIndex () + 1); }

    //////////////////////////////////////////////////////////////////////////////
    /// Open the custom context menu
    ///
    /// @param[in]  pos     Position to open menu at
    ///
    //////////////////////////////////////////////////////////////////////////////
    void openMenu (const QPoint& pos);

    //////////////////////////////////////////////////////////////////////////////
    /// Open a dialog to rename this port
    ///
    //////////////////////////////////////////////////////////////////////////////
    void rename ();

    };
} // namespace ui::sensors