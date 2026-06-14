/**
 * @file        clock/clockpanel.cpp
 * @brief       Panel for the fast clock/automation configuration tab
 * @author      Justin Scott
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/clock/analogclock.hpp>
#include <ui/clock/automation.hpp>
#include <ui/clock/clockpanel.hpp>
#include <ui/clock/config.hpp>


#include <QGroupBox>
#include <QBoxLayout>

namespace ui::clock
{


ClockPanel::ClockPanel (control::ControllerManager& controllers,
                        control::AutomationManager& automations,
                        QWidget*                    parent) :
    QWidget (parent)
    {
    m_clockBox = new QGroupBox{ this };

    QVBoxLayout*    layout      = new QVBoxLayout{ this };
    QHBoxLayout*    clockLayout = new QHBoxLayout{ m_clockBox };

    clockLayout->addWidget (new AnalogClock{ this });
    clockLayout->addWidget (new ConfigForm{ this });

    m_clockBox->setLayout (clockLayout);

    layout->addWidget (m_clockBox, 0, Qt::AlignTop);
    layout->addWidget (new AutomationGroup{ controllers, automations, this }, 0, Qt::AlignTop);

    layout->setAlignment (Qt::AlignTop);

    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);
    setTitles ();
    setLayout (layout);
    }


void ClockPanel::setTitles ()
    {
    m_clockBox->setTitle (tr ("Clock Settings"));
    }

} // namespace ui::clock