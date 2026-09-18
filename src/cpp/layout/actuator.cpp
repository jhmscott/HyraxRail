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
#include <layout/utils.hpp>

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

actuatorIcon Actuator::getIcon () const
    LAYOUT_DEFINE_GETTER (m_icon, NO_ICON)

actuatorMode Actuator::getMode () const
    LAYOUT_DEFINE_GETTER (m_mode, actuatorMode::SWITCH)

std::string Actuator::getName () const
    LAYOUT_DEFINE_GETTER (m_name)

void Actuator::setIcon (actuatorIcon icon)
    LAYOUT_DEFINE_SETTER (m_icon, setActuatorIcon, icon)

void Actuator::setName (const std::string& name)
    LAYOUT_DEFINE_SETTER (m_name, setActuatorName, name)

void Actuator::setMode (actuatorMode mode)
    LAYOUT_DEFINE_SETTER (m_mode, setActuatorMode, mode)

void Actuator::setAddress (uint address)
    LAYOUT_DEFINE_SETTER (m_address, setActuatorAddress, address)

void Actuator::setDuration (uint duration)
    LAYOUT_DEFINE_SETTER (m_duration, setActuatorDuration, duration)

void Actuator::set (bool val)
    {
    if (m_state->m_state != val)
        {
        m_controller->setActuator (m_id, val);
        m_state->m_state = val;
        callAll (&Actuator::stateChanged, val);
        }
    }

uint Actuator::getDuration () const
    LAYOUT_DEFINE_GETTER (m_duration, 0)

bool Actuator::get () const
    LAYOUT_DEFINE_GETTER (m_state, false)

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

uint Actuator::getAddress () const
    LAYOUT_DEFINE_GETTER (m_address, 0)

}
