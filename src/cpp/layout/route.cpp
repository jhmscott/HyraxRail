/**
 * @file        layout/route.cpp
 * @brief       Provides an interface for controlling a train route
 * @author      Justin Scott
 * @date        2026-03-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <layout/route.hpp>

namespace layout
{
Route::Route (RouteController*      controller,
              const std::string&    name,
              const routeList&      members,
              size_t                id) :
    ComponentDerived<RouteController> (controller, id),
    m_name (name),
    m_members (members)
    {}

void Route::setName (const std::string& name)
    {
    setAll (&Route::m_name, name);
    m_controller->setRouteName (m_id, name);
    }

void Route::setActuators (const routeList& members)
    {
    setAll (&Route::m_members, members);
    m_controller->setRouteMembers (m_id, members);
    }

void Route::set ()
    {
    m_controller->setRoute (m_id);
    }

void Route::remove ()
    {
    m_controller->removeRoute (m_id);
    deregister ();
    }

void Route::request () { m_controller->requestRouteControl (m_id); }

void Route::release () { m_controller->releaseRouteControl (m_id); }

}
