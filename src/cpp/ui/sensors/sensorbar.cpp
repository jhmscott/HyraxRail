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
SensorBar::SensorBar (const layout::SensorModule& module, QWidget* parent) :
    common::LayoutButton (parent),
    m_module (module)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };
    QHBoxLayout* states = new QHBoxLayout{ this };

    m_states.reserve (m_module.size ());

    for (size_t ii = 0; ii < m_module.size (); ++ii)
        {
        states->addWidget (
            m_states.emplace_back (
                new SensorManageState{ m_module, ii, 5, this}));
        }

    setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    layout->addLayout (states);
    layout->addWidget (new QLabel{ module.getName ().c_str (), this}, 0, Qt::AlignHCenter);

    setLayout (layout);
    }

} // namespace ui::sensors
