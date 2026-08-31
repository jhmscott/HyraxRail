/**
 * @file        layout/actautor.cpp
 * @brief       Provides an interface for an actuator/
 *              switching device used in a model train layout
 * @author      Justin Scott
 * @date        2026-03-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <layout/actuator.hpp>

namespace layout
{
Actuator::Actuator (ActuatorController* controller,
                    const std::string&  name,
                    actuatorIcon        icon,
                    actuatorMode        mode,
                    uint                address,
                    uint                duration,
                    size_t              id,
                    bool                state) :
    Base (controller,
          id,
          std::make_shared<actuatorState>
                (actuatorState{ name, icon, state, mode, address, duration }))
    {}

void Actuator::setIcon (actuatorIcon icon)
    {
    m_controller->setActuatorIcon (m_id, icon);
    m_state->m_icon = icon;
    }

void Actuator::setName (const std::string& name)
    {
    m_controller->setActuatorName (m_id, name);
    m_state->m_name = name;
    }

void Actuator::setMode (actuatorMode mode)
    {
    m_controller->setActuatorMode (m_id, mode);
    m_state->m_mode = mode;
    }

void Actuator::setAddress (uint address)
    {
    m_controller->setActuatorAddress (m_id, address);
    m_state->m_address = address;
    }

void Actuator::setDuration (uint duration)
    {
    m_controller->setActuatorDuration (m_id, duration);
    m_state->m_duration = duration;
    }

void Actuator::set (bool val)
    {
    if (m_state->m_state != val)
        {
        m_controller->setActuator (m_id, val);
        m_state->m_state = val;
        callAll (&Actuator::stateChanged, val);
        }
    }

void Actuator::request ()
    {
    if (NULL != m_controller)
        {
        m_controller->requestActuatorControl (m_id);
        }
    }

void Actuator::release ()
    {
    if (NULL != m_controller)
        {
        m_controller->releaseActuatorControl (m_id);
        }
    }

void Actuator::remove ()
    {
    m_controller->removeActuator (m_id);
    destroyThis ();
    deregister ();
    }

}
