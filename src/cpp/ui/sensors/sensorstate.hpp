/**
 * @file        sensors/sensorstate.hpp
 * @brief       Widget to display the state of a sensor port
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/sensor.hpp>

#include <ui/common/gesture.hpp>

#include <QWidget>

namespace ui::sensors
{
//////////////////////////////////////////////////////////////////////////////
/// Widget to display the state of a sensor port
///
//////////////////////////////////////////////////////////////////////////////
class SensorDisplayState : public QWidget
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
    SensorDisplayState (const layout::SensorModule& module,
                        size_t                      idx,
                        uint                        radius,
                        QWidget*                    parent);

    //////////////////////////////////////////////////////////////////////////////
    /// Set the notification indicator for this sensor
    ///
    /// @param[in]  notified        True if notification are enabled
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setNotified (bool notified);

    //////////////////////////////////////////////////////////////////////////////
    /// Get whether the notification indicator is enabled for this sensor
    ///
    /// @return     True        True if notifications are enabled
    ///
    //////////////////////////////////////////////////////////////////////////////
    bool getNotified () const { return m_notified; }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the index of sensor port within module
    ///
    /// @return     Sensor port index
    ///
    //////////////////////////////////////////////////////////////////////////////
    size_t getIndex () const { return m_idx; }

protected:
    //////////////////////////////////////////////////////////////////////////////
    /// Paint this widget
    ///
    /// @param[in]  event       Paint event
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void paintEvent (QPaintEvent* event) override;

private:
    layout::SensorModule    m_module;   ///< Module this sensor port belongs to
    size_t                  m_idx;      ///< Index of port within that module
    uint                    m_radius;   ///< Radius of the indicator in pixels
    bool                    m_notified; ///< True if notifications are enabled
    bool                    m_state;    ///< Current state of the sensor port (on/off)

    //////////////////////////////////////////////////////////////////////////////
    /// Handle the sensor state change signal
    ///
    /// @param[in]  idx     Sensor port index
    /// @param[in]  value   New sensor value
    ///
    //////////////////////////////////////////////////////////////////////////////
    void stateChanged (size_t idx, bool value);
    };

} // namespace ui::sensors
