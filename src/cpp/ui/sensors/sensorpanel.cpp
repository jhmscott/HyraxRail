/**
 * @file        sensors/sensorpanel.cpp
 * @brief       Panel to display the state of sensors
 * @author      Justin Scott
 * @date        2026-03-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/sensors/sensorbar.hpp>
#include <ui/sensors/sensorpanel.hpp>

namespace ui::sensors
{
SensorPanel::SensorPanel (QWidget* parent) :
    QWidget (parent)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    layout->addWidget (new SensorBar{ "Module 1", this }, 0, Qt::AlignTop);

    setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    setLayout (layout);
    }
}
