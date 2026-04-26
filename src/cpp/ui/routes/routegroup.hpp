/**
 * @file        routes/routegroup.hpp
 * @brief       Group box triggering routes. One per controller
 * @author      Justin Scott
 * @date        2026-03-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/controllers/base.hpp>

#include <ui/common/autogrid.hpp>

#include <QGroupBox>

namespace ui::routes
{
//////////////////////////////////////////////////////////////////////////////
/// Group box containing all the routes for a controller
///
//////////////////////////////////////////////////////////////////////////////
class RouteGroup : public QGroupBox
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller      Controller to display routes for
    /// @param[in]  parent          Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    RouteGroup (control::ControllerBase& controller, QWidget* parent);

private:
    control::ControllerBase*    m_controller;   ///< Controller
    common::AutoGridLayout*     m_gridLayout;   ///< Layout containing route buttons

    //////////////////////////////////////////////////////////////////////////////
    /// Add a route button to the grid layout
    ///
    /// @param[in]  route       Route to create button for
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addRouteToGrid (const layout::Route& route);

private slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Prompt the user to create a new route
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addRoute ();

    //////////////////////////////////////////////////////////////////////////////
    /// Remove route from the controller and the panel
    ///
    //////////////////////////////////////////////////////////////////////////////
    void removeRoute ();
    };

}