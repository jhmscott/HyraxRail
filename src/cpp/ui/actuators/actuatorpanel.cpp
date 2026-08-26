/**
 * @file        actuators/actuatorpanel.hpp
 * @brief       Panel for the actuator tab of the main window
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/actuators/actuatorpanel.hpp>
#include <ui/actuators/actuatorgroup.hpp>

#include <ui/common/utils.hpp>

#include <utils/algorithm.hpp>

#include <QGroupBox>

namespace ui::actuators
{
ActuatorPanel::ActuatorPanel (control::ControllerManager&   controllers,
                              control::AutomationManager&   automations,
                              QWidget*                      parent) :
    QWidget (parent),
    m_controllers (controllers),
    m_automations (automations)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    for (control::ControllerBase& controller : m_controllers)
        {
        layout->addWidget (new ActuatorGroup{ automations, controller, this });
        }

    layout->setAlignment (Qt::AlignTop);

    connect (&m_controllers,
             &control::ControllerManager::controllerAdded,
              this,
             &ActuatorPanel::addController);

    connect (&m_controllers,
             &control::ControllerManager::controllerDeleted,
              this,
             &ActuatorPanel::remove);

    setLayout (layout);
    }

void ActuatorPanel::remove (const control::ControllerBase& controller)
    {
    ptrdiff_t idx = m_controllers.indexOf (controller);

    if (idx >= 0)
        {
        common::removeWidgetFromLayout (*layout (), idx);
        }
    }

void ActuatorPanel::addController (control::ControllerBase& controller)
    {
    QVBoxLayout* myLayout = static_cast<QVBoxLayout*> (layout ());

    myLayout->addWidget (new ActuatorGroup{ m_automations, controller, this });
    }

}