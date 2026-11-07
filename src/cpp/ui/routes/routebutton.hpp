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
#include <control/controllers/base.hpp>

#include <layout/route.hpp>

#include <ui/common/gesture.hpp>
#include <ui/common/utils.hpp>

#include <ui/common/layout/button.hpp>

#include <ui/lang.hpp>

#include <QLabel>
#include <QWidget>

namespace ui::routes
{
//////////////////////////////////////////////////////////////////////////////
/// Button to control a route
///
//////////////////////////////////////////////////////////////////////////////
class RouteButton : public common::ComponentButton<layout::Route>
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  route       Route to control
    /// @param[in]  controller  Controller for route
    /// @param[in]  automations List of automations
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    RouteButton (const layout::Route&           route,
                 control::ControllerBase&       controller,
                 control::AutomationManager&    automations,
                 QWidget*                       parent);

private:
    control::ControllerBase&    m_controller;
    QLabel*                     m_name;         ///< Name of the route

    //////////////////////////////////////////////////////////////////////////////
    /// Update the tooltips to match the cuurrent route state
    ///
    //////////////////////////////////////////////////////////////////////////////
    void updateTooltip ();

    UILANG_ON_CHANGE (QWidget, updateTooltip ());
private slots:

    //////////////////////////////////////////////////////////////////////////////
    /// Opens the dialog to edit this route
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void editComponent () override;

    //////////////////////////////////////////////////////////////////////////////
    /// Get the translated text for the delete dialog title
    ///
    /// @return     Delete dialog title
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual QString getDeleteText () const override { return tr ("Delete Route"); }
    };

} // namespace ui::routes
