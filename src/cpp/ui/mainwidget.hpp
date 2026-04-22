/**
 * @file        mainwidget.hpp
 * @brief       Top level widget
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/actuators/actuatorpanel.hpp>
#include <ui/config/configpanel.hpp>
#include <ui/routes/routepanel.hpp>
#include <ui/trains/dualcontrol.hpp>

#include <QSettings>
#include <QTabWidget>

namespace ui
{

///////////////////////////////////////////////////////////////////////////////
/// Main widget, which comprises the client area of the main widget
///
///////////////////////////////////////////////////////////////////////////////
class MainWidget : public QTabWidget
    {
    Q_OBJECT;
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent          Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    MainWidget (QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~MainWidget ();
private:
    control::ControllerManager*     m_controllers;  ///< List of controllers
    actuators::ActuatorPanel*       m_actuators;    ///< Actuator control widget
    trains::DualControlWidget*      m_trains;       ///< Train control widget
    config::ConfigPanel*            m_cfg;          ///< Main settings widget
    routes::RoutePanel*             m_routes;       ///< Route control widget

    };

} // namespace ui
