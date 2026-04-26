/**
 * @file        actuators/actuatorbutton.cpp
 * @brief       Layout switching device toggle button
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/actuators/actuatorbutton.hpp>

#include <QBoxLayout>
#include <QLabel>

namespace ui::actuators
{
ActuatorButton::ActuatorButton (const layout::Actuator& actuator, QWidget* parent, bool dummy) :
    QWidget (parent),
    m_actuator (actuator),
    m_dummy (dummy)
    {
    static const std::pair<QIcon, QIcon> icons[] =
        {
            {
            QIcon{ ":/icons/switches/left-track-left.svg"       },
            QIcon{ ":/icons/switches/left-track-straight.svg"   }
            },
            {
            QIcon{ ":/icons/switches/right-track-right.svg"     },
            QIcon{ ":/icons/switches/right-track-straight.svg"  }
            }
        };

    auto icon = icons[actuator.getIcon ()];

    QVBoxLayout* layout = new QVBoxLayout{ this };

    m_button = new common::IconToggle
        {
        icon.first,
        icon.second,
        false,
        parent
        };

    m_button->setChecked (actuator.get ());

    QLabel* label = new QLabel{ actuator.getName ().c_str (), this };

    m_button->setFixedSize (50, 50);
    m_button->setIconSize (QSize{ 30, 30 });

    label->setAlignment (Qt::AlignTop);

    layout->addWidget (m_button);
    layout->addWidget (label, 0, Qt::AlignHCenter);

    setContentsMargins (0, 20, 0, 0);

    connect (m_button,
            &QPushButton::toggled,
             this,
            &ActuatorButton::onToggle);

    setLayout (layout);

    if (not dummy)
        {
        m_actuator.request ();
        }
    }

void ActuatorButton::setState (bool state)
    {
    m_button->setChecked (state);

    if (not m_dummy)
        {
        m_actuator.set (state);
        }
    }

void ActuatorButton::onToggle (bool state)
    {
    if (not m_dummy)
        {
        m_actuator.set (state);
        }
    }
}