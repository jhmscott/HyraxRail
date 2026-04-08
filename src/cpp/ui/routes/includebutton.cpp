/**
 * @file        routes/includebutton.cpp
 * @brief       Actuator button for use with the EditRouteDialog.
 *              Includes both the toggle for the actuator itself, and
 *              switch to indicate if it should be included in the route
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/routes/includebutton.hpp>

#include <QBoxLayout>

namespace ui::routes
{
ActuatorIncludeButton::ActuatorIncludeButton (const layout::Actuator& actuator, QWidget* parent) :
    QWidget (parent),
    m_actuator (actuator)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    m_btn       = new actuators::ActuatorButton{ actuator, this, true };
    m_switch    = new common::ToggleSwitch{ this };

    layout->addWidget (m_btn);
    layout->addWidget (m_switch, 0, Qt::AlignHCenter);

    connect (m_switch,
            &common::ToggleSwitch::switched,
             this,
            &ActuatorIncludeButton::includeSwitched);

    setContentsMargins (0, 0, 0, 0);

    setLayout (layout);
    }
}
