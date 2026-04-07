/**
 * @file        layout/route.hpp
 * @brief       Provides an interface for controlling a train route
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <layout/base.hpp>

#include <string>

namespace layout
{

class RouteController;


class Route : public ComponentDerived<RouteController>
    {
public:
    Route (RouteController*     controller,
           const std::string&   name,
           size_t               id);

    std::string getName () const { return m_name; }

    void set ();

    void request ();

    void release ();

private:
    std::string m_name;
    bool        m_state;
    };


class RouteController : public ControllerBase<Route>
    {
    friend class Route;
public:
    virtual void setRoute (size_t id) = 0;

    virtual void requestRouteControl (size_t id) = 0;

    virtual void releaseRouteControl (size_t id) = 0;
    };



}