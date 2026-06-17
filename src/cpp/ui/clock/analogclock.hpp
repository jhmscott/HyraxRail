/**
 * @file        clock/analogclock.hpp
 * @brief       Analog clock widget, based on code from Qt Widgets Examples
 *              Tutorial : https://doc.qt.io/qt-6/qtwidgets-widgets-analogclock-example.html
 *              Code : https://code.qt.io/cgit/qt/qtbase.git/tree/examples/widgets/widgets/analogclock?h=6.11
 *              Copyright (C) 2016 The Qt Company Ltd.
 *              SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
 * @author      Justin Scott
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/clock/clock.hpp>

#include <QWidget>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Analog clock widget, renders the current state of the fast clock
///
///////////////////////////////////////////////////////////////////////////////
class AnalogClock : public ClockWidget
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit AnalogClock (QWidget* parent);

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// Handle painting of the widget
    ///
    /// @param[in]  event       Paint event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void paintEvent (QPaintEvent* event) override;

private:
    QFont m_font;   ///< Font used for day window

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the widget's time
    ///
    /// @param[in]  time        Simulated time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setTime (const control::FastClock::time_point& time) { update (); }
    };

} // namespace ui::clock
