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
    CLOCK_TYPE_ANALOG_BASIC,    ///< Analog clock
    CLOCK_TYPE_ANALOG_SUB,      ///< Analog clock
    CLOCK_TYPE_ANALOG_SNOWBANK, ///< Analog clock

    NUM_ANALOG_TYPES,           ///< Delimeter only

    CLOCK_TYPE_DIGITAL_BASIC =  ///< Digital clock
                    NUM_ANALOG_TYPES,

    NUM_CLOCK_TYPES             ///< Delimiter only
    };

constexpr bool isAnalog (clockStyle style)
    {
    return style >= CLOCK_TYPE_ANALOG_BASIC &&
           style <  NUM_ANALOG_TYPES;
    }


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

    ///////////////////////////////////////////////////////////////////////////////
    /// Get this clock's style
    ///
    /// @return     Clock style
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual clockStyle getStyle () const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the clock style
    ///
    /// @param[in]  newStyle    New clock style
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setStyle (clockStyle style) = 0;

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