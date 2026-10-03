/**
 * @file        virtual/vroute.hpp
 * @brief       Software defined route controller
 * @author      Justin Scott
 * @date        2026-10-01
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <layout/route.hpp>
#include <layout/virtual/base.hpp>

namespace layout
{

class VirtualRouteController :
    public RouteController,
    public VirtualControllerBase
    {
public:
    explicit VirtualRouteController (ActuatorController& actuatorController) {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the routes configured for this controller. A route is a group of
    /// actuators and associated state that can be triggered
    ///
    /// @return     List of routes
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::vector<Route> getRoutes () const override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a route on  the controller
    ///
    /// @param[in]  name               Name of the route
    /// @param[in]  actuators   List of actuators and the state to set them to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Route createRoute (const std::string&   name,
                               const routeList&     actuators) override;
private:
    std::vector<Route> m_routes;

    ///////////////////////////////////////////////////////////////////////////////
    /// Activate a route
    ///
    /// @param[in]  id      Unique ID of route to activate
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setRoute (size_t id) override;

    // State is managed within the route, with no need to write it back to a controller,
    // so all of the following are no-ops

    //////////////////////////////////////////////////////////////////////////////
    /// Remove a route from the controller
    ///
    /// @param[in]  id      ID of route to remove
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void removeRoute (size_t id) override {}

    //////////////////////////////////////////////////////////////////////////////
    /// Set the components of the route
    ///
    /// @param[in]  id      ID of route
    /// @param[in]  members Actuators/states comprising the route
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void setRouteMembers (size_t id, const routeList& members) override {}

    //////////////////////////////////////////////////////////////////////////////
    /// Set the route's name
    ///
    /// @param[in]  id      Route's ID
    /// @param[in]  name    Route name
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void setRouteName (size_t id, const std::string& name) override {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Request control of a route
    ///
    /// @param[in]  id      Unique ID of route to request control of
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void requestRouteControl (size_t id) override {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Release control of a route
    ///
    /// @param[in]  id      Unique ID of route to release control of
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void releaseRouteControl (size_t id) override {}
    };

} // namespace layout