/**
 * @file        clock/digitalclock.hpp
 * @brief       Digital clock widget
 * @author      Justin Scott
 * @date        2026-06-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/clock.hpp>

#include <ui/clock/clock.hpp>

#include <QWidget>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Digital clock widget
///
///////////////////////////////////////////////////////////////////////////////
class DigitalClock : public ClockWidget
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit DigitalClock (QWidget* parent);

private:
    QLabel* m_time;     ///< Time label (excluding seconds)
    QLabel* m_seconds;  ///< Seconds label
    QLabel* m_date;     ///< Date label
    QLabel* m_amPm;     ///< AM/PM indicator

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the widget's time
    ///
    /// @param[in]  time        Simulated time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setTime (const control::FastClock::time_point& time) override;
    };

} // namespace ui::clock