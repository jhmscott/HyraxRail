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

    ///////////////////////////////////////////////////////////////////////////////
    /// Get this clock's style
    ///
    /// @return     Clock style
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual clockStyle getStyle () const override { return CLOCK_TYPE_DIGITAL_BASIC; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the clock style
    ///
    /// @param[in]  newStyle    New clock style
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setStyle (clockStyle newStyle) override {}
private:
    QLabel* m_time;             ///< Time label (excluding seconds)
    QLabel* m_seconds;          ///< Seconds label
    QLabel* m_date;             ///< Date label
    QLabel* m_amPm;             ///< AM/PM indicator
    bool    m_showColon = true; ///< Show the colon

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the widget's time
    ///
    /// @param[in]  time        Simulated time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setTime (const control::FastClock::time_point& time) override;

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the stop
    ///
    /// @param[in]  time        Simulated time we were stopped at
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void stopped (const control::FastClock::time_point& time);
    };

} // namespace ui::clock