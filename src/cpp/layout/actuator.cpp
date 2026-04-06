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
                    size_t              id,
                    bool                state) :
    ComponentDerived<ActuatorController> (controller, id),
    m_name (name),
    m_icon (icon),
    m_state (state)
    {}

void Actuator::set (bool val) { m_controller->setActuator (m_id, val); }

void Actuator::request () { m_controller->requestActuatorControl (m_id); }

void Actuator::release () { m_controller->releaseActuatorControl (m_id); }

}
