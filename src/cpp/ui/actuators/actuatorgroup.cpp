/**
 * @file        actuators/actuatorgroupbox.cpp
 * @brief       Actuator groupbox, one per controller
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/actuators/actuatorbutton.hpp>
#include <ui/actuators/actuatorgroup.hpp>
#include <ui/common/autogrid.hpp>

namespace ui::actuators
{
ActuatorGroup::ActuatorGroup (control::ControllerBase&  controller,
                              QWidget*                  parent) :
    QGroupBox (controller.getFriendlyName ().c_str (), parent),
    m_controller (&controller)
    {
    common::AutoGridLayout* layout = new common::AutoGridLayout{ common::AutoGridLayout::expand::ROW_FIRST, 5, this };
    auto actuators = m_controller->getActuators ();

    for (const layout::Actuator& actuator : actuators)
        {
        ActuatorButton* btn = new ActuatorButton{ actuator, this };

        btn->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);
        layout->addWidget (btn);
        }

    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);

    layout->setAlignment (Qt::AlignTop | Qt::AlignLeft);

    setLayout (layout);
    }
}
