/**
 * @file        routes/rouetebutton.hpp
 * @brief       Button to control the state and configuration of a route
 * @author      Justin Scott
 * @date        2026-04-24
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/manager.hpp>

#include <layout/route.hpp>

#include <ui/common/gesture.hpp>
#include <ui/common/utils.hpp>
#include <ui/lang.hpp>

#include <QLabel>
#include <QWidget>

namespace ui::routes
{
//////////////////////////////////////////////////////////////////////////////
/// Button to control a route
///
//////////////////////////////////////////////////////////////////////////////
class RouteButton : public common::HoldToOpenMenu<QWidget>
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  route       Route to control
    /// @param[in]  automations List of automations
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    RouteButton (const layout::Route&           route,
                 control::AutomationManager&    automations,
                 QWidget*                       parent);


signals:
    //////////////////////////////////////////////////////////////////////////////
    /// Signals the user has requested this route be deleted
    ///
    //////////////////////////////////////////////////////////////////////////////
    void routeDeleted ();

private:
    layout::Route               m_route;        ///< Route being controlled
    control::AutomationManager& m_automations;
    QLabel*                     m_name;         ///< Name of the route

    //////////////////////////////////////////////////////////////////////////////
    /// Update the tooltips to match the cuurrent route state
    ///
    //////////////////////////////////////////////////////////////////////////////
    void updateTooltip ();

    //////////////////////////////////////////////////////////////////////////////
    /// Create the context menu actions
    ///
    //////////////////////////////////////////////////////////////////////////////
    void createMenu ();

    UILANG_ON_CHANGE (QWidget,
        {
        common::removeAllActions (*this);
        createMenu ();
        updateTooltip ();
        });

private slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Prompts the user if they want this route removed, and signals deletion if
    /// accepted
    ///
    //////////////////////////////////////////////////////////////////////////////
    void removeRoute ();

    //////////////////////////////////////////////////////////////////////////////
    /// Opens the dialog to edit this route
    ///
    //////////////////////////////////////////////////////////////////////////////
    void editRoute ();
    };

} // namespace ui::routes