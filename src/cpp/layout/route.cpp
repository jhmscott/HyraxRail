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
              size_t                id) :
    ComponentDerived<RouteController> (controller, id),
    m_name (name)
    {}


void Route::set ()
    {
    m_controller->setRoute (m_id);
    }

void Route::request () { m_controller->requestRouteControl (m_id); }

void Route::release () { m_controller->releaseRouteControl (m_id); }

}
