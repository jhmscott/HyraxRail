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
#include <ui/clock/digitalclock.hpp>

#include <QGroupBox>
#include <QBoxLayout>

namespace ui::clock
{


ClockPanel::ClockPanel (control::ControllerManager& controllers,
                        control::AutomationManager& automations,
                        QWidget*                    parent) :
    QWidget (parent)
    {
    QVBoxLayout*    layout      = new QVBoxLayout{ this };
    ConfigForm*     form        = new ConfigForm{ this };

    m_clockBox      = new QGroupBox{ this };
    m_clockLayout   = new QHBoxLayout{ m_clockBox };

    m_clockLayout->addWidget (new AnalogClock{ this });
    m_clockLayout->addWidget (form);
    m_clockLayout->setContentsMargins (0, 9, 0, 9);

    m_clockBox->setLayout (m_clockLayout);

    layout->addWidget (m_clockBox, 0, Qt::AlignTop);
    layout->addWidget (new AutomationGroup{ controllers, automations, this }, 0, Qt::AlignTop);

    layout->setAlignment (Qt::AlignTop);

    connect (form,
            &ConfigForm::styleChanged,
             this,
            &ClockPanel::styleChanged);

    setContentsMargins (0, 9, 0, 9);
    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);
    setTitles ();
    setLayout (layout);
    }


void ClockPanel::setTitles ()
    {
    m_clockBox->setTitle (tr ("Clock Settings"));
    }


void ClockPanel::styleChanged (clockStyle style)
    {
    ClockWidget* newClock = NULL;
    QWidget*     oldClock = m_clockLayout->itemAt (0)->widget ();

    switch (style)
        {
        case CLOCK_TYPE_DIGITAL:
            {
            newClock = new DigitalClock{ this };
            break;
            }
        case CLOCK_TYPE_ANALOG:
            {
            newClock = new AnalogClock{ this };
            break;
            }
        }

    m_clockLayout->replaceWidget (oldClock, newClock);

    delete oldClock;
    }
} // namespace ui::clock