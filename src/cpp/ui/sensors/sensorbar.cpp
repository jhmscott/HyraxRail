/**
 * @file        sensors/sensorbar.spp
 * @brief       Bar representing the state of sensors for a given sensor module
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/sensors/sensorbar.hpp>

#include <QLabel>

namespace ui::sensors
{
SensorBar::SensorBar (const QString& name, QWidget* parent) :
    QWidget (parent)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };
    QHBoxLayout* states = new QHBoxLayout{ this };

    m_states.reserve (8);

    for (int ii = 0; ii < 8; ++ii)
        {
        states->addWidget (
            m_states.emplace_back (
                new SensorState{ 5, 0 == (ii % 2), ii, this }));
        }

    setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    layout->addLayout (states);
    layout->addWidget (new QLabel{ name, this }, 0, Qt::AlignHCenter);

    setLayout (layout);
    }
}