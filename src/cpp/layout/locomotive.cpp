/**
 * @file        layout/locomotive.cpp
 * @brief       Provides an interface for controlling a locomotive
 * @author      Justin Scott
 * @date        2026-03-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <layout/locomotive.hpp>
#include <layout/utils.hpp>

namespace layout
{

Locomotive::Locomotive (LocomotiveController*           controller,
                        const std::string&              name,
                        trackProtocol                   proto,
                        uint                            address,
                        const std::vector<funcInfo>&    functions,
                        size_t                          id) :
    Base (controller,
          id,
          std::make_shared<locomotiveState> (locomotiveState{ name, proto, address, functions }))
    {}

std::string Locomotive::getName () const
    LAYOUT_DEFINE_GETTER (locomotiveState::m_name)

void Locomotive::setName (const std::string& name)
    LAYOUT_DEFINE_SETTER (m_name, setLocomotiveName, name)

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

trackProtocol Locomotive::getProtocol () const
    LAYOUT_DEFINE_GETTER (m_proto, TRACK_PROTO_UNKNOWN);

std::vector<funcInfo> Locomotive::getFunctions () const
    LAYOUT_DEFINE_GETTER (m_functions)

void Locomotive::setProtocol (trackProtocol protocol)
    LAYOUT_DEFINE_SETTER (m_proto, setLocomotiveProtocol, protocol)

void Locomotive::setAddress (uint address)
    LAYOUT_DEFINE_SETTER (m_address, setLocomotiveAddress, address)

void Locomotive::setFunctions (const std::vector<funcInfo>& functions)
    LAYOUT_DEFINE_SETTER (m_functions, setLocomotiveFunctions, functions)

void Locomotive::remove ()
    {
    m_controller->removeLocomotive (m_id);
    destroyThis ();
    deregister ();
    }

QString funcInfo::uiName () const
    {
    QString friendlyName;

    if (not name.empty ())
        {
        friendlyName = Locomotive::tr ("Function %1 : %2").arg (id).arg (name.c_str ());
        }
    // Function 0 is pretty universally the main headlight
    else if (0 == id)
        {
        static constexpr const int id0 = 0;

        friendlyName = Locomotive::tr ("Function %1 : %2").
                                         arg (id0).arg (Locomotive::tr ("Headlights"));
        }
    else
        {
        friendlyName = Locomotive::tr ("Function %1").arg (id);
        }

    return friendlyName;
    }

uint Locomotive::getAddress () const LAYOUT_DEFINE_GETTER (m_address, 0);

}
