/**
 * @file        virtual/vroute.hpp
 * @brief       Software defined route controller
 * @author      Justin Scott
 * @date        2026-10-01
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <layout/virtual/vroute.hpp>

namespace layout
{
std::vector<Route> VirtualRouteController::getRoutes () const
    {
    std::vector<Route> routes;

    routes.reserve (m_routes.size ());

    std::copy_if (m_routes.begin (),
                  m_routes.end (),
                  std::back_inserter (routes),
                  [] (const Route& route) -> bool
                  { return route; });

    return routes;
    }

Route VirtualRouteController::createRoute (const std::string&   name,
                                            const routeList&    actuators)
    {
    auto it = std::find_if_not (m_routes.begin (),
                                m_routes.end (),
                                [] (const Route& route) -> bool
                                { return route; });
    size_t id;

    if (m_routes.end () == it)
        {
        id = m_routes.size ();
        }
    else
        {
        id = std::distance (m_routes.begin (), it);
        }

    // ID of 0 has special meaning, start at 1
    ++id;

    Route route{ this, name, actuators, id };

    if (m_routes.end () == it)
        {
        m_routes.push_back (route);
        }
    else
        {
        *it = route;
        }

    return route;
    }

void VirtualRouteController::setRoute (size_t id)
    {
    if (id < m_routes.size ())
        {
        auto it = m_routes.begin ();

        std::advance (it, id);

        for (auto& [actuator, state] : it->getActuators ())
            {
            actuator.set (state);
            }
        }
    }
} // namespace layout