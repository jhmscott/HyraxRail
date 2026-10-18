/**
 * @file        layout/sensor.hpp
 * @brief       Sensor that can detect the position of a train on the tracks
 * @author      Justin Scott
 * @date        2026-10-16
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/base.hpp>

#include <mutex>

#ifdef LAYOUT_TEST_CLASS
class LAYOUT_TEST_CLASS;
#endif // LAYOUT_TEST_CLASS

namespace layout
{

class SensorController;
class SensorModule;

/// Represents the state of the sensor module
struct sensorState
    {
    ///////////////////////////////////////////////////////////////////////////////
    /// Construct from sensor values
    ///
    /// @param[in]  values      Sensor values
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit sensorState (std::vector<bool>&& values, std::string&& name) :
        m_values (std::move (values)),
        m_name (std::move (name))
        {}

    std::vector<bool>   m_values;   ///< Sensor values
    std::string         m_name;
    mutable std::mutex  m_mtx;      ///< Protection for sensor values
    };

///////////////////////////////////////////////////////////////////////////////
/// Represents a groups of sensors
///
///////////////////////////////////////////////////////////////////////////////
class SensorModule : public ComponentDerived<SensorController, sensorState>
    {
    Q_OBJECT

    friend class SensorController;
public:
    /// Base class type
    using Base = ComponentDerived<SensorController, sensorState>;

    using Base::ComponentDerived;

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller  Sensor controller
    /// @param[in]  values      Sensor values
    /// @param[in]  id          Sensor component ID
    ///
    ///////////////////////////////////////////////////////////////////////////////
    SensorModule (SensorController* controller,
                  std::vector<bool> values,
                  std::string       name,
                  size_t            id);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the number of sensors in this module
    ///
    /// @return     Number of sensors
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t size () const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Index operator - get the state of the n-th sensor in the module
    ///
    /// @param[in]  idx     Index off sensor in the module to get the state of
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool operator[] (size_t idx) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the sensor module's name
    ///
    /// @return     Module name
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string getName () const { return m_state->m_name; }
signals:
    ///////////////////////////////////////////////////////////////////////////////
    /// Signals the state of a sensor has changed
    ///
    /// @param[in]  idx     Index of sensor in this module
    /// @param[in]  value   New state of the sensor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void sensorChanged (size_t idx, bool value);
    };

///////////////////////////////////////////////////////////////////////////////
/// Controller for a set of sensor modules
///
///////////////////////////////////////////////////////////////////////////////
class SensorController : public ControllerBase<SensorModule>
    {

#ifdef LAYOUT_TEST_CLASS
    friend class ::LAYOUT_TEST_CLASS;
#endif // LAYOUT_TEST_CLASS

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the sensors managed by this controller
    ///
    /// @return     List of sensor modules
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::vector<SensorModule> getSensors () const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a sensor module
    ///
    /// @param[in]  num     Number of sensors in the module
    ///
    /// @return     Newly created sensor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual SensorModule createSensor (size_t num) = 0;

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// Update the state of a sensor module
    ///
    /// @param[in,out]  module      Module to update
    /// @param[in]      values      New module values
    ///
    /// @remarks        Call from network thread when sensor state changes. Thread safe
    ///////////////////////////////////////////////////////////////////////////////
    void setSensorState (SensorModule& module, const std::vector<bool>& values);

    };

} // namespace layout
