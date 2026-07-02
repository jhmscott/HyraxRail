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

#include <utils/draw.hpp>

#include <QWidget>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Analog clock widget, renders the current state of the fast clock
///
///////////////////////////////////////////////////////////////////////////////
class AnalogClock : public ClockWidget, public utils::draw::DebugDrawable
    {
public:
    // Analog clock style
    enum style
        {
        STYLE_BASIC,    ///< Basic clock style
        STYLE_SUB,      ///< Sub style dive watch

        NUM_STYLES      ///< Delimiter only
        };

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit AnalogClock (QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get this clock's style
    ///
    /// @return     Clock style
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual clockStyle getStyle () const override
        { return static_cast<clockStyle> (m_style); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the clock style
    ///
    /// @param[in]  newStyle    New clock style
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setStyle (clockStyle newStyle) override;

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// Handle painting of the widget
    ///
    /// @param[in]  event       Paint event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void paintEvent (QPaintEvent* event) override;

private:
    QFont m_font;               ///< Font used for day window
    style m_style = STYLE_BASIC;///< Current style

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the widget's time
    ///
    /// @param[in]  time        Simulated time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setTime (const control::FastClock::time_point& time) { update (); }
    };

} // namespace ui::clock
