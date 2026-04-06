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

#include <ui/actuators/actuatorpanel.hpp>
#include <ui/config/configpanel.hpp>
#include <ui/routes/routepanel.hpp>
#include <ui/trains/dualcontrol.hpp>

#include <QSettings>
#include <QTabWidget>

namespace ui
{

class MainWidget : public QTabWidget
    {
    Q_OBJECT;
public:
    MainWidget (QWidget* parent);

    ~MainWidget ();
private:
    control::controllerOwnerList    m_controllers;
    actuators::ActuatorPanel*       m_actuators;
    trains::DualControlWidget*      m_trains;
    config::ConfigPanel*            m_cfg;
    routes::RoutePanel*             m_routes;

    void controllerDeleted (control::ControllerBase& controller);

    void controllerAdded (const control::createControllerInfo& controller);
    };

}