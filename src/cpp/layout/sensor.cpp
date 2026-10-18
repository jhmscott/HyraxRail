/**
 * @file        layout/sensor.cpp
 * @brief       Sensor that can detect the position of a train on the tracks
 * @author      Justin Scott
 * @date        2026-10-16
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <layout/sensor.hpp>

#include <utils/algorithm.hpp>

namespace layout
{
SensorModule::SensorModule (SensorController*   controller,
                            std::vector<bool>   values,
                            std::string         name,
                            size_t              id) :
    Base (controller, id, std::make_shared<sensorState> (std::move (values), std::move (name)))
    {
    }

size_t SensorModule::size () const
    {
    std::lock_guard lk{ m_state->m_mtx };
    return m_state->m_values.size ();
    }

bool SensorModule::operator[] (size_t idx) const
    {
    std::lock_guard lk{ m_state->m_mtx };
    return m_state->m_values[idx];
    }

void SensorController::setSensorState (SensorModule& module, const std::vector<bool>& values)
    {
    std::vector<size_t> changed;

    {
    std::lock_guard lk{ module.m_state->m_mtx };

    changed = utils::algorithm::mismatches (module.m_state->m_values, values);

    module.m_state->m_values = values;
    }

    for (size_t idx : changed)
        {
        module.callAll (&SensorModule::sensorChanged, idx, values[idx]);
        }
    }

} // namespace layout
