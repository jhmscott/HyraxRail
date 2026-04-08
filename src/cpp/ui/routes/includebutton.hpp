/**
 * @file        routes/includebutton.hpp
 * @brief       Actuator button for use with the EditRouteDialog.
 *              Includes both the toggle for the actuator itself, and
 *              switch to indicate if it should be included in the route
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/actuator.hpp>

#include <ui/actuators/actuatorbutton.hpp>
#include <ui/common/toggleswitch.hpp>

#include <QWidget>

namespace ui::routes
{

class ActuatorIncludeButton : public QWidget
    {
    Q_OBJECT
public:
    ActuatorIncludeButton (const layout::Actuator& actuator, QWidget* parent);

    bool isIncluded () const { return m_switch->getSwitch (); }

    bool actuatorState () const { return m_btn->getState (); };

    layout::Actuator getActuator () const { return m_actuator; }
signals:
    void includeSwitched (bool included);
private:
    layout::Actuator            m_actuator;
    common::ToggleSwitch*       m_switch;
    actuators::ActuatorButton*  m_btn;
    };
}