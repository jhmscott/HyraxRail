/**
 * @file        actuators/actuatorpanel.hpp
 * @brief       Panel for the actuator tab of the main window
 * @author      Justin Scott
 * @date        2026-02-01
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/actuators/actuatorbutton.hpp>

#include <ui/common/autogrid.hpp>


#include <QGroupBox>
#include <QWidget>

namespace ui::actuators
{
class ActuatorPanel : public QWidget
    {
public:
    ActuatorPanel (control::ControllerManager* controllers, QWidget* parent);

private:
    control::ControllerManager* m_controllers;

private slots:
    void remove (const control::ControllerBase& controller);

    void add (control::ControllerBase& controller);
    };
}