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
              size_t                id,
              bool                  state) :
    ComponentDerived<RouteController> (controller, id),
    m_name (name),
    m_state (state)
    {}


void Route::set (bool val)
    {
    m_controller->setRoute (m_id, val);
    m_state = val;
    }

void Route::request () { m_controller->requestRouteControl (m_id); }

void Route::release () { m_controller->releaseRouteControl (m_id); }

}
