/**
 * @file        actuators/actuatordelegate.cpp
 * @brief       Delegate for actuator components
 * @author      Justin Scott
 * @date        2026-10-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/actuators/actuatorbutton.hpp>
#include <ui/actuators/actuatordelegate.hpp>
#include <ui/actuators/editactuator.hpp>

namespace ui::actuators
{
common::LayoutButton* ActuatorDelegate::openCreate (control::ControllerBase&  controller,
                                                    QWidget*                  parent)
    {
    common::LayoutButton*   btn     = NULL;
    layout::Actuator        actuator;

    EditActuatorDialog dlg{ parent };

    if (QDialog::Accepted == dlg.exec ())
        {
        actuator = controller.createActuator (dlg.getName (),
                                              dlg.getAddress (),
                                              dlg.getIcon (),
                                              dlg.getMode (),
                                              dlg.getDuration ());
        btn = makeButton (actuator);
        }
    
    return btn;
    }

void ActuatorDelegate::createButtons (control::ControllerBase&                    controller,
                                      std::function<void (common::LayoutButton*)> addToLayout)
    {
    for (const layout::Actuator& actuator : controller.getActuators ())
        {
        if (layout::NO_ICON != actuator.getIcon ())
            {
            addToLayout (makeButton (actuator));
            }
        }
    }

common::LayoutButton* ActuatorDelegate::makeButton (const layout::Actuator& actuator)
    {
    ActuatorButton* btn = new ActuatorButton{ actuator, NULL, false, &m_automations};

    btn->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    return btn;
    }
}