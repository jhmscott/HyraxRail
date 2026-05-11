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

///////////////////////////////////////////////////////////////////////////////
/// Speed controller widget with stop button, slider bar and speed indicator
///
///////////////////////////////////////////////////////////////////////////////
class SpeedControlWidget : public QWidget
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    SpeedControlWidget (QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the locomotive to control
    ///
    /// @param[in]  loco        Locomtive to control
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLocomotive (const layout::Locomotive& loco);

    ///////////////////////////////////////////////////////////////////////////////
    /// Clear the locomotive under control
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clear ();
private:
    layout::Locomotive  m_loco;     ///< Locomotive under control
    QSlider*            m_slider;   ///< Speed slider bar
    QLabel*             m_label;    ///< Speed indicator
    QPushButton*        m_stop;     ///< Stop button (not emergency stop)

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the slider value changing
    ///
    /// @param[in]  value       New slider value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onSliderChange (int value)
        {
        m_loco.setSpeed (static_cast<int8_t> (value));
        m_label->setText (QString::asprintf ("%02d", abs (value * 99 / 127)));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the stop being pressed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onStopButton () { m_slider->setValue (0); }
    };

} // namespace ui::trains
