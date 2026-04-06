/**
 * @file        actuators/actuatorgroupbox.hpp
 * @brief       Actuator groupbox, one per controller
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/controllers/base.hpp>

#include <QGroupBox>

namespace ui::actuators
{

class ActuatorGroup : public QGroupBox
    {
public:
    ActuatorGroup (control::ControllerBase& controller, QWidget* parent);

    control::ControllerBase* getController () { return m_controller; }
private:
    control::ControllerBase* m_controller;
    };

}