/**
 * @file        layout/route.cpp
 * @brief       Provides an interface for controlling a train route
 * @author      Justin Scott
 * @date        2026-03-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <layout/route.hpp>
#include <layout/utils.hpp>

namespace layout
{
Route::Route (RouteController*      controller,
              const std::string&    name,
              const routeList&      members,
              size_t                id) :
    Base (controller,
          id,
          std::make_shared<routeState> (routeState{ name, members }))
    {}

std::string Route::getName () const
    LAYOUT_DEFINE_GETTER (m_name)

void Route::setName (const std::string& name)
    LAYOUT_DEFINE_SETTER (m_name, setRouteName, name)

void Route::setActuators (const routeList& members)
    LAYOUT_DEFINE_SETTER (m_members, setRouteMembers, members)

void Route::set ()
    {
    m_controller->setRoute (m_id);
    }

routeList Route::getActuators () const
    LAYOUT_DEFINE_GETTER (m_members)

void Route::remove ()
    {
    m_controller->removeRoute (m_id);
    destroyThis ();
    deregister ();
    }

void Route::request () { m_controller->requestRouteControl (m_id); }

void Route::release () { m_controller->releaseRouteControl (m_id); }

}
