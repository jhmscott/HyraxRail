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

///////////////////////////////////////////////////////////////////////////////
/// Draw a clock hand
///
/// @tparam     N       Number of hand points
///
/// @Param[in,out]  painter     Painter instance
/// @param[in]      rotation    Hand rotation in degrees [0,360]
/// @param[in]      offfset     Offset to apply to hand
/// @param[in]      hand        Clock hand vector symbol
/// @param[in]      lolipop     Draw a "lolipop" style hand, with a large
///                             circle on the end
///
///////////////////////////////////////////////////////////////////////////////
template<size_t N>
static void drawHand (QPainter&     painter,
                      double        rotation,
                      const QPoint& offset,
                      const QPoint(&hand)[N],
                      bool          lolipop)
    {
    QPainterStateGuard guard{ &painter };

    painter.rotate (rotation);
    painter.translate (offset);
    painter.drawConvexPolygon (hand, N);

    if (lolipop)
        {
        painter.drawEllipse (-3, -3, 6, 6);
        painter.drawEllipse (-5, -68, 10, 10);
        }
    }


///////////////////////////////////////////////////////////////////////////////
/// Draw a clock hand with a "shadow" to provide contrast with hands below it
///
/// @tparam     N       Number of hand points
///
/// @Param[in,out]  painter     Painter instance
/// @param[in]      rotation    Hand rotation in degrees [0,360]
/// @param[in]      handColor   Colour to draw hand in
/// @param[in]      shadowColor Colour of hand shadow
/// @param[in]      hand        Clock hand vector symbol
/// @param[in]      lolipop     Draw a "lolipop" style hand, with a large
///                             circle on the end
///
///////////////////////////////////////////////////////////////////////////////
template<size_t N>
static void drawHandWithShadow (QPainter&       painter,
                                double          rotation,
                                const QColor&   handColor,
                                const QColor&   shadowColor,
                                const QPoint  (&hand)[N],
                                bool            lolipop = false)
    {
    painter.setPen (Qt::NoPen);
    painter.setBrush (shadowColor);

    drawHand (painter, rotation, {  0,  1 }, hand, lolipop);
    drawHand (painter, rotation, {  0, -1 }, hand, lolipop);
    drawHand (painter, rotation, {  1,  0 }, hand, lolipop);
    drawHand (painter, rotation, { -1,  0 }, hand, lolipop);


    painter.setBrush (handColor);
    drawHand (painter, rotation, { 0, 0 }, hand, lolipop);
    }

AnalogClock::AnalogClock (QWidget* parent) :
    ClockWidget (parent)
    {
    m_font.setFamily ("Cascadia Mono");
    m_font.setPixelSize (14);
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

    const QColor hourColor      = palette ().color (QPalette::Text);
    const QColor minuteColor    = palette ().color (QPalette::Text);
    const QColor secondsColor   = palette ().color (QPalette::Accent);
    const QColor shadowColor    = Qt::gray;

    int side = std::min (width(), height());

    QPainter painter{ this };

    // Setup painting parameters
    painter.setRenderHint (QPainter::Antialiasing);
    painter.translate (width () / 2, height () / 2);
    painter.scale (side / 200.0, side / 200.0);

    QDateTime   dateTime    = control::FastClock::qDateTime ();
    QTime       time        = dateTime.time ();
    QDate       date        = dateTime.date ();

    // Draw the day/date window

    painter.setFont (m_font);
    painter.drawText (18,
                      5,
                      QString{ "%1|%2 "}.arg (
                          utils::time::dayOfWeekAbreviation (
                              static_cast<utils::time::dayOfTheWeek> (date.dayOfWeek () - 1)),
                          QString::number (date.day ())));
    painter.drawRoundedRect (QRect{ 15, -9, 55, 18 }, 3, 3);

    // Draw hour hand

    drawHandWithShadow (painter,
                        30.0 * ((time.hour () + time.minute () / 60.0)),
                        hourColor,
                        shadowColor,
                        hourHand);

    // Draw hour indices

    for (int i = 0; i < 12; ++i)
        {
        painter.drawRect (73, -3, 16, 6);
        painter.rotate (30.0);
        }

    // Draw minute hand

    drawHandWithShadow (painter,
                        6.0 * (time.minute () + time.second () / 60.0),
                        minuteColor,
                        shadowColor,
                        minuteHand);

    // Draw seconds hands

    drawHandWithShadow (painter,
                        6.0 * time.second (),
                        secondsColor,
                        shadowColor,
                        secondsHand,
                        true);

    // Draw minute/second indices

    painter.setPen (minuteColor);

    for (int j = 0; j < 60; ++j)
        {
        painter.drawLine (92, 0, 96, 0);
        painter.rotate (6.0);
        }

    }

} // namespace ui::clock
