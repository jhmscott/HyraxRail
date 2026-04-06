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

#include "notifygroup.hpp"

#include <QApplication>
#include <QBoxLayout>
#include <QLabel>

namespace ui::config
{
NotifyGroup::NotifyGroup (QWidget* parent) :
    QGroupBox ("Notifications", parent)
    {
    QVBoxLayout*    layout  = new QVBoxLayout{ this };
    QFont           font    = QApplication::font ();

    font.setPixelSize (16);

    QHBoxLayout*    stopNotify  = new QHBoxLayout{ this };
    QLabel*         stopLabel   = new QLabel{ "Global Stop", this};

    stopLabel->setFont (font);

    stopNotify->addWidget (stopLabel, 0, Qt::AlignLeft);
    stopNotify->addWidget (new common::ToggleSwitch{ this }, 0, Qt::AlignRight);

    QHBoxLayout*    sensorNotify    = new QHBoxLayout{ this };
    QLabel*         sensorLabel     = new QLabel{ "Sensors", this};

    sensorLabel->setFont (font);

    sensorNotify->addWidget (sensorLabel, 0, Qt::AlignLeft);
    sensorNotify->addWidget (new common::ToggleSwitch{ this }, 0, Qt::AlignRight);

    layout->addLayout (stopNotify);
    layout->addWidget (new common::Separator{ this });
    layout->addLayout (sensorNotify);

    setLayout (layout);
    }
}
