/**
 * @file        clock/analogclock.cpp
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


#include <control/automation/clock.hpp>

#include <ui/clock/analogclock.hpp>

#include <QPainter>
#include <QPainterStateGuard>
#include <QTime>

namespace ui::clock
{
AnalogClock::AnalogClock (QWidget* parent) :
    QWidget (parent)
    {
    auto& clock = control::FastClock::instance ();

    // Force a repaint on a clock tick...
    connect (&clock,
             &control::FastClock::tick,
              this,
              QOverload<>::of (&AnalogClock::update));

    // ... or a manual update
    connect (&clock,
             &control::FastClock::timeChanged,
             this,
             QOverload<>::of (&AnalogClock::update));
    }

void AnalogClock::paintEvent (QPaintEvent* event)
    {

    static const QPoint hourHand[4] =
        {
            {  5,  14 },
            { -5,  14 },
            { -4, -71 },
            {  4, -71 }
        };

    static const QPoint minuteHand[4] =
        {
            {  4,  14 },
            { -4,  14 },
            { -3, -89 },
            {  3, -89 }
        };

    static const QPoint secondsHand[4] =
        {
            {  1,  14 },
            { -1,  14 },
            { -1, -89 },
            {  1, -89 }
        };

    const QColor hourColor      = palette().color(QPalette::Text);
    const QColor minuteColor    = palette().color(QPalette::Text);
    const QColor secondsColor   = palette().color(QPalette::Accent);

    int side = std::min (width(), height());

    QPainter painter{ this };

    // Setup painting parameters
    painter.setRenderHint (QPainter::Antialiasing);
    painter.translate (width () / 2, height () / 2);
    painter.scale (side / 200.0, side / 200.0);

    QTime time = control::FastClock::qDateTime ().time ();

    // Draw hour hand

    painter.setPen (Qt::NoPen);
    painter.setBrush (hourColor);

    {
    QPainterStateGuard guard{ &painter };

    painter.rotate (30.0 * ((time.hour() + time.minute() / 60.0)));
    painter.drawConvexPolygon (hourHand, std::size (hourHand));
    }

    // Draw hour indices

    for (int i = 0; i < 12; ++i)
        {
        painter.drawRect (73, -3, 16, 6);
        painter.rotate (30.0);
        }

    // Draw minute hand

    painter.setBrush(minuteColor);

    {
    QPainterStateGuard guard{ &painter };

    painter.rotate (6.0 * (time.minute () + time.second () / 60.0));
    painter.drawConvexPolygon (minuteHand, std::size (minuteHand));
    }

    // Draw seconds hands

    painter.setBrush(secondsColor);

    {
    QPainterStateGuard guard{ &painter };

    painter.rotate (6.0 * time.second());
    painter.drawConvexPolygon (secondsHand, std::size (secondsHand));
    painter.drawEllipse (-3, -3, 6, 6);
    painter.drawEllipse (-5, -68, 10, 10);
    }

    // Draw minute/second indices

    painter.setPen (minuteColor);

    for (int j = 0; j < 60; ++j)
        {
        painter.drawLine(92, 0, 96, 0);
        painter.rotate(6.0);
        }
    }

} // namespace ui::clock
