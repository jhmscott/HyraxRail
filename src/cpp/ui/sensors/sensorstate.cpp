/**
 * @file        sensors/sensorstate.cpp
 * @brief       Widget to display the state of sensors
 * @author      Justin Scott
 * @date        2026-03-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/sensors/sensorstate.hpp>

#include <QApplication>
#include <QPainter>
#include <QStyleHints>

namespace ui::sensors
{
SensorDisplayState::SensorDisplayState (const layout::SensorModule& module,
                                        size_t                      idx,
                                        uint                        radius,
                                        QWidget*                    parent) :
    QWidget (parent),
    m_module (module),
    m_idx (idx),
    m_radius (radius),
    m_notified (false),
    m_state (m_module[idx])
    {
    static constexpr uint PADDING = 4;

    setFixedSize (radius * 2 + PADDING,
                  radius * 2 + PADDING);

    connect (&m_module,
             &layout::SensorModule::sensorChanged,
              this,
             &SensorDisplayState::stateChanged);
    }

void SensorDisplayState::paintEvent (QPaintEvent* event)
    {
    QPainter        painter{ this };
    Qt::GlobalColor color   = m_state ? Qt::green : Qt::red;
    Qt::GlobalColor outline;
    QPoint          center  = rect ().center ();
    bool            darkMode= Qt::ColorScheme::Dark == qApp->styleHints ()->colorScheme ();

    if (darkMode)
        {
        outline = m_notified ? Qt::cyan : Qt::white;
        }
    else
        {
        outline = m_notified ? Qt::blue : Qt::black;
        }

    center.rx () += 1;
    center.ry () += 1;

    painter.setRenderHint (QPainter::Antialiasing, true);
    painter.setPen (QPen{ QBrush { outline }, m_notified ? 2.0 : 1.0 });
    painter.setBrush (color);

    painter.drawEllipse (center, m_radius + 1, m_radius + 1);
    }

void SensorDisplayState::stateChanged (size_t idx, bool value)
    {
    if (idx == m_idx)
        {
        m_state = value;

        // redraw
        update ();
        }
    }

void SensorDisplayState::setNotified (bool notified)
    {
    m_notified = notified;

    // Redraw
    update ();
    }

}
