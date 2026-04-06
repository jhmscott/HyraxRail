/**
 * @file        actuators/actuatorbutton.hpp
 * @brief       Layout switching device toggle button
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/icontoggle.hpp>

#include <layout/actuator.hpp>

namespace ui::actuators
{
class ActuatorButton : public QWidget
    {
public:
    ActuatorButton (const layout::Actuator& actuator, QWidget* parent);

    ~ActuatorButton ()
        {
        // m_actuator.release ();
        }

private:
    layout::Actuator m_actuator;

    void onToggle (bool state)
        {
        m_actuator.set (state);
        }
    };
}