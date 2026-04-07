/**
 * @file        trains/speedcontrol.hpp
 * @brief       Slider bar to control the speed of a locomotive
 * @author      Justin Scott
 * @date        2026-01-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/locomotive.hpp>

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>

namespace ui::trains
{

class SpeedControlWidget : public QWidget
    {
public:

    SpeedControlWidget (QWidget* parent);

    void setLocomotive (const layout::Locomotive& loco);

    void clear ();
private:
    layout::Locomotive  m_loco;
    QSlider*            m_slider;
    QLabel*             m_label;
    QPushButton*        m_stop;

    void onSliderChange (int value)
        {
        m_loco.setSpeed (static_cast<int8_t> (value));
        m_label->setText (QString::asprintf ("%02d", abs (value * 99 / 127)));
        }

    void onStopButton ()
        {
        m_slider->setValue (0);
        }
    };
}
