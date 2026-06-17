/**
 * @file        clock/clock.hpp
 * @brief       Base class for clock widgets
 * @author      Justin Scott
 * @date        2026-06-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/clock.hpp>

#include <ui/lang.hpp>

#include <QWidget>

namespace ui::clock
{

// Clock widget style
enum clockStyle
    {
    CLOCK_TYPE_ANALOG,  ///< Analog clock
    CLOCK_TYPE_DIGITAL, ///< Digital clock

    NUM_CLOCK_TYPES     ///< Delimiter only
    };

///////////////////////////////////////////////////////////////////////////////
/// Base class for clock widgets
///
///////////////////////////////////////////////////////////////////////////////
class ClockWidget : public QWidget
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ClockWidget (QWidget* parent);

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Set the widget's time
    ///
    /// @param[in]  time        Simulated time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setTime (const control::FastClock::time_point& time) = 0;

    UILANG_ON_CHANGE (QWidget, setTime (control::FastClock::now ()));
    };

} // namespace ui::clock