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
#include <ui/actuators/editactuator.hpp>

#include <ui/common/seperator.hpp>
#include <ui/common/utils.hpp>

#include <QLabel>
#include <QVBoxLayout>

namespace ui::actuators
{
ActuatorGroup::ActuatorGroup (control::ControllerBase&  controller,
                              QWidget*                  parent) :
    QGroupBox (controller.getFriendlyName ().c_str (), parent),
    m_controller (&controller)
    {
    QVBoxLayout*            layout      = new QVBoxLayout{ this };
    QHBoxLayout*            addLayout   = new QHBoxLayout{ this };

    m_gridLayout  = new common::AutoGridLayout
                        {
                        common::AutoGridLayout::expand::ROW_FIRST,
                        common::AutoGridLayout::EXPAND,
                        this
                        };

    for (const layout::Actuator& actuator : m_controller->getActuators ())
        {
        if (layout::NO_ICON != actuator.getIcon ())
            {
            addActuatorToGrid (actuator);
            }
        }

    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);

    m_gridLayout->setAlignment (Qt::AlignTop | Qt::AlignLeft);

    common::PointedButton* addBtn = new common::PointedButton
                                        {
                                        QIcon{ ":/icons/misc/plus.svg" },
                                        "",
                                        this
                                        };

    common::makeFrameless (*addBtn);

    addLayout->addWidget (addBtn, 0, Qt::AlignLeft);
    addLayout->addWidget (new QLabel{ "Add Switching Item", this }, 0, Qt::AlignLeft);
    addLayout->setAlignment (Qt::AlignLeft);

    layout->addItem (m_gridLayout);
    layout->addWidget (new common::Separator{ this });
    layout->addLayout (addLayout);

    connect (addBtn,
            &QPushButton::released,
             this,
            &ActuatorGroup::addActuator);

    setLayout (layout);
    }

void ActuatorGroup::addActuatorToGrid (const layout::Actuator& actuator)
    {
    ActuatorButton* btn = new ActuatorButton{ actuator, this };

    btn->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    connect (btn,
            &ActuatorButton::actuatorDeleted,
             this,
            &ActuatorGroup::removeActuator);

    m_gridLayout->addWidget (btn);
    }

void ActuatorGroup::removeActuator ()
    {
    ActuatorButton* btn         = static_cast<ActuatorButton*> (QObject::sender ());
    QRect           geometry    = m_gridLayout->geometry ();

    m_gridLayout->removeWidget (btn);

    btn->deleteLater ();

    // Force a refresh
    m_gridLayout->setGeometry (geometry);
    }

void ActuatorGroup::addActuator ()
    {
    layout::Actuator actuator;

    {
    EditActuatorDialog dlg{ *m_controller, this };

    if (QDialog::Accepted == dlg.exec ())
        {
        actuator = m_controller->createActuator (dlg.getName (),
                                                 dlg.getAddress (),
                                                 dlg.getIcon (),
                                                 dlg.getMode (),
                                                 dlg.getDuration ());
        }
    }

    if (actuator)
        {
        addActuatorToGrid (actuator);

        m_gridLayout->itemAt (m_gridLayout->count () - 1)->widget ()->show ();

        // Force a refresh
        m_gridLayout->setGeometry (m_gridLayout->geometry ());
        }
    }
}
