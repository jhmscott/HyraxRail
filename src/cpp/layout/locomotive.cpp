/**
 * @file        layout/locomotive.cpp
 * @brief       Provides an interface for controlling a locomotive
 * @author      Justin Scott
 * @date        2026-03-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <layout/locomotive.hpp>

namespace layout
{

Locomotive::Locomotive (LocomotiveController*   controller,
                        const std::string&      name,
                        trackProtocol           proto,
                        uint                    address,
                        size_t                  id) :
    Base (controller,
          id,
          std::make_shared<locomotiveState> (locomotiveState{ name, proto, address }))
    {}

void Locomotive::setName (const std::string& name)
    {
    m_controller->setLocomotiveName (m_id, name);
    m_state->m_name = name;
    }

void Locomotive::setSpeed (int8_t speed)
    {
    if (NULL != m_controller)
        {
        m_controller->setSpeed (m_id, speed);
        }
    }

void Locomotive::requestControl ()
    {
    if (NULL != m_controller)
        {
        m_controller->requestControl (m_id);
        }
    }

void Locomotive::releaseControl ()
    {
    if (NULL != m_controller)
        {
        m_controller->releaseControl (m_id);
        }
    }

void Locomotive::setFunc (uint8_t func, bool enable)
    {
    if (NULL != m_controller)
        {
        m_controller->setFunc (m_id, func, enable);
        }
    }

std::vector<funcInfo> Locomotive::getFunctions () const
    {
    return NULL != m_controller ? m_controller->getFunctions (m_id) : std::vector<funcInfo>();
    }

void Locomotive::setProtocol (trackProtocol protocol)
    {
    m_controller->setLocomotiveProtocol (m_id, protocol);
    m_state->m_proto = protocol;
    }

void Locomotive::setAddress (uint address)
    {
    m_controller->setLocomotiveAddress (m_id, address);
    m_state->m_address = address;
    }

void Locomotive::remove ()
    {
    m_controller->removeLocomotive (m_id);
    destroyThis ();
    deregister ();
    }
}
