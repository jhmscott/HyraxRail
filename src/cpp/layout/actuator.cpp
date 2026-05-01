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
    ComponentDerived<ActuatorController> (controller, id),
    m_name (name),
    m_icon (icon),
    m_state (state),
    m_mode (mode),
    m_address (address),
    m_duration (duration)
    {}

void Actuator::setIcon (actuatorIcon icon)
    {
    m_controller->setActuatorIcon (m_id, icon);
    setAll (&Actuator::m_icon, icon);
    }

void Actuator::setName (const std::string& name)
    {
    m_controller->setActuatorName (m_id, name);
    setAll (&Actuator::m_name, name);
    }

void Actuator::setMode (actuatorMode mode)
    {
    m_controller->setActuatorMode (m_id, mode);
    setAll (&Actuator::m_mode, mode);
    }

void Actuator::setAddress (uint addrress)
    {
    m_controller->setActuatorAddress (m_id, addrress);
    setAll (&Actuator::m_address, addrress);
    }

void Actuator::setDuration (uint duration)
    {
    m_controller->setActuatorAddress (m_id, duration);
    setAll (&Actuator::m_duration, duration);
    }

void Actuator::set (bool val) { m_controller->setActuator (m_id, val); }

void Actuator::request () { m_controller->requestActuatorControl (m_id); }

void Actuator::release () { m_controller->releaseActuatorControl (m_id); }

void Actuator::remove ()
    {
    m_controller->removeActuator (m_id);
    deregister ();
    }

}
