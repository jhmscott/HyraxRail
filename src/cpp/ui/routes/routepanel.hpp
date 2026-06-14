/**
 * @file        routes/routepanel.hpp
 * @brief       Panel for the routes tab of the main widget
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/manager.hpp>
#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/common/autogrid.hpp>

#include <QPushButton>
#include <QScrollArea>
#include <QWidget>


namespace ui::routes
{

//////////////////////////////////////////////////////////////////////////////
/// Tab containing route information
///
//////////////////////////////////////////////////////////////////////////////
class RoutePanel : public QWidget
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllers     List of controllers
    /// @param[in]  automations     List of automations
    /// @param[in]  parent          Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    RoutePanel (control::ControllerManager& controllers,
                control::AutomationManager& automations,
                QWidget*                    parent);

private:
    control::ControllerManager& m_controllers;  ///< List of controllers
    control::AutomationManager& m_automations;  ///< List of automations

private slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Remove a controller from this panel
    ///
    /// @param[in]  controller      Controller to remove
    ///
    //////////////////////////////////////////////////////////////////////////////
    void remove (const control::ControllerBase& controller);

    //////////////////////////////////////////////////////////////////////////////
    /// Add a controller to this panel
    ///
    /// @param[in]  controller      Controller to add
    ///
    //////////////////////////////////////////////////////////////////////////////
    void add (control::ControllerBase& controller);

    };

}