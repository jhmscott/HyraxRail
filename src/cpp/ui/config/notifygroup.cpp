/**
 * @file        config/notifygroup.cpp
 * @brief       Group box for the configuration of notifications
 * @author      Justin Scott
 * @date        2026-04-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/seperator.hpp>
#include <ui/common/toggleswitch.hpp>

#include <ui/config/notifygroup.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QLabel>

namespace ui::config
{
NotifyGroup::NotifyGroup (QWidget* parent) :
    QGroupBox (parent)
    {
    QVBoxLayout*    layout  = new QVBoxLayout{ this };
    QFont           font    = QApplication::font ();

    font.setPixelSize (16);

    QHBoxLayout*    stopNotify  = new QHBoxLayout{ this };

    m_stopLabel   = new QLabel{ this };

    m_stopLabel->setFont (font);

    stopNotify->addWidget (m_stopLabel, 0, Qt::AlignLeft);
    stopNotify->addWidget (new common::ToggleSwitch{ this }, 0, Qt::AlignRight);

    QHBoxLayout*    sensorNotify    = new QHBoxLayout{ this };

    m_sensorLabel     = new QLabel{ this };

    m_sensorLabel->setFont (font);

    sensorNotify->addWidget (m_sensorLabel, 0, Qt::AlignLeft);
    sensorNotify->addWidget (new common::ToggleSwitch{ this }, 0, Qt::AlignRight);

    layout->addLayout (stopNotify);
    layout->addWidget (new common::Separator{ this });
    layout->addLayout (sensorNotify);

    setLabels ();
    setLayout (layout);
    }

void NotifyGroup::setLabels ()
    {
    setTitle (tr ("Notifications"));

    m_stopLabel->setText (tr ("Global Stop"));
    m_sensorLabel->setText (tr ("Sensors"));
    }
}
