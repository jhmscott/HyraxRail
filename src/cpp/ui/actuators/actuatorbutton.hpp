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
    ActuatorButton (const layout::Actuator& actuator, QWidget* parent, bool dummy = false);

    bool getState () { return m_button->isChecked (); }

private:
    layout::Actuator    m_actuator;
    common::IconToggle* m_button;
    bool                m_dummy;

    void onToggle (bool state);
    };
}