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
ActuatorButton::ActuatorButton (const layout::Actuator& actuator, QWidget* parent) :
    QWidget (parent),
    m_actuator (actuator)
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

    common::IconToggle* button = new common::IconToggle
        {
        icon.first,
        icon.second,
        false,
        parent
        };

    button->setChecked (actuator.get ());

    QLabel* label = new QLabel{ actuator.getName ().c_str (), this };

    button->setFixedSize (50, 50);
    button->setIconSize (QSize{ 30, 30 });

    label->setAlignment (Qt::AlignTop);

    layout->addWidget (button);
    layout->addWidget (label, 0, Qt::AlignHCenter);

    setContentsMargins (0, 20, 0, 0);

    connect (button,
             &QPushButton::toggled,
             this,
             &ActuatorButton::onToggle);

    setLayout (layout);

    m_actuator.request ();
    }
}