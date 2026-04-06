/**
 * @file        actuators/actuatorpanel.hpp
 * @brief       Panel for the actuator tab of the main window
 * @author      Justin Scott
 * @date        2026-02-01
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/autogrid.hpp>

#include <ui/actuators/actuatorbutton.hpp>

#include <control/controllers/base.hpp>

#include <QGroupBox>
#include <QWidget>

namespace ui::actuators
{
class ActuatorPanel : public QWidget
    {
public:
    ActuatorPanel (const control::controllerList& controllers, QWidget* parent);

    void remove (const control::ControllerBase& controller);

    void add (control::ControllerBase& controller);
private:
    control::controllerList m_controllers;
    };
}