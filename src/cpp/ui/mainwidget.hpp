/**
 * @file        mainwidget.hpp
 * @brief       Top level widget
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/manager.hpp>
#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/actuators/actuatordelegate.hpp>
#include <ui/common/schemeicon.hpp>
#include <ui/routes/routedelegate.hpp>
#include <ui/sensors/sensordelegate.hpp>

#include <ui/lang.hpp>

#include <QSettings>
#include <QTabWidget>

namespace ui
{

///////////////////////////////////////////////////////////////////////////////
/// Main widget, which comprises the client area of the main widget
///
///////////////////////////////////////////////////////////////////////////////
class MainWidget : public common::SchemeTabWidget
    {
    Q_OBJECT;
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllers     List of controllers
    /// @param[in]  parent          Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    MainWidget (control::ControllerManager& controllers, QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~MainWidget ();
private:
    control::ControllerManager&     m_controllers;  ///< List of controllers
    control::AutomationManager*     m_automations;  ///< List of automations
    actuators::ActuatorDelegate     m_actuatorDel;  ///< Delegate for actuator panel
    routes::RouteDelegate           m_routeDel;     ///< Delegate for route panel
    sensors::SensorDelegate         m_sensorDel;    ///< Delegate for sensor panel

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the tab tooltips
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTooltips ();

    UILANG_ON_CHANGE (common::SchemeTabWidget, setTooltips ())
    };

} // namespace ui
