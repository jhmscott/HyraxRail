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
ActuatorPanel::ActuatorPanel (const control::controllerList& controllers, QWidget* parent) :
    QWidget (parent),
    m_controllers (controllers)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    for (control::ControllerBase* controller : m_controllers)
        {
        layout->addWidget (new ActuatorGroup{ *controller, this });
        }

    layout->setAlignment (Qt::AlignTop);

    setLayout (layout);
    }

void ActuatorPanel::remove (const control::ControllerBase& controller)
    {
    auto it = std::find (m_controllers.begin (),
                         m_controllers.end (),
                        &controller);

    if (m_controllers.end () != it)
        {
        common::removeWidgetFromLayout (*layout (),
                                        std::distance (m_controllers.begin (),
                                                       it));
        }

    utils::algorithm::erase (m_controllers, &controller);
    }

void ActuatorPanel::add (control::ControllerBase& controller)
    {
    QVBoxLayout* myLayout = static_cast<QVBoxLayout*> (layout ());

    m_controllers.push_back (&controller);

    myLayout->addWidget (new ActuatorGroup{ controller, this });
    }

}