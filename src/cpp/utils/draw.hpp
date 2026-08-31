/**
 * @file        utils/draw.hpp
 * @brief       Utilities for 2D drawing/painting
 * @author      Justin Scott
 * @date        2026-06-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <utils/poly.hpp>

#include <QElapsedTimer>
#include <QPainter>

#include <deque>

namespace utils::draw
{


///////////////////////////////////////////////////////////////////////////////
/// Draw a polygon
///
/// @param[in,out]  painter     Painter to draw with
/// @param[in]      poly        Polygon to draw
///
///////////////////////////////////////////////////////////////////////////////
void polygon (QPainter& painter, utils::PolygonViewF poly);


///////////////////////////////////////////////////////////////////////////////
/// Adds state and logic for drawing debug information about a 2D widget being
/// painted. Use with ScopedDebugDraw in your painting/drawing function to
/// record the time
///
///////////////////////////////////////////////////////////////////////////////
class DebugDrawable
    {
    friend class ScopedDebugDraw;
private:
    /// Records time info about a frame
    struct frameInfo
        {
        std::chrono::system_clock::time_point   time    =   ///< Time the frame was drawn
                                std::chrono::system_clock::now ();
        std::chrono::nanoseconds                duration;   ///< How long it took to draw

        ///////////////////////////////////////////////////////////////////////////////
        /// Records a frame for the current time
        ///
        /// @param[in]  duration        Time it took to draw the frame
        ///
        ///////////////////////////////////////////////////////////////////////////////
        implicit frameInfo (std::chrono::nanoseconds duration) :
            duration (duration)
            {}
        };

    std::deque<frameInfo>   m_frames;   ///< List of last N frames,
                                        ///  Used to calculate a rolling average

    ///////////////////////////////////////////////////////////////////////////////
    /// Draw the debug info for this widget
    ///
    /// @param[in,out]  paintable   Item to paint to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void debugDraw (QPaintDevice& paintable);
    };


///////////////////////////////////////////////////////////////////////////////
/// Use within your painting logic to record when the frame was drawn and how
/// long it took
///
///////////////////////////////////////////////////////////////////////////////
class ScopedDebugDraw
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in,out]  painter     Device to be painted to
    /// @param[in,out]  drawable    Widget being debugged
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ScopedDebugDraw (QPaintDevice& painter, DebugDrawable& drawable);

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @tparam         T           Type of paintable being debugged. Must derive
    ///                             from both QPaintDevice and DebugDrawable
    ///
    /// @param[in,out]  paintable   Item to draw debug info to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class T>
    explicit ScopedDebugDraw (T& paintable) :
        ScopedDebugDraw (paintable, paintable)
        {
        static_assert (std::is_base_of_v<QPaintDevice, T> &&
                       std::is_base_of_v<DebugDrawable, T>,
                       "paintable must derive from QPaintDevice and DebugDrawable");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor. Records metrics and draws the info
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~ScopedDebugDraw ();

    // non-copyable
    ScopedDebugDraw (const ScopedDebugDraw&) = delete;
    ScopedDebugDraw& operator= (const ScopedDebugDraw&) = delete;

    // non-movable
    ScopedDebugDraw (ScopedDebugDraw&&) = delete;
    ScopedDebugDraw& operator= (ScopedDebugDraw&&) = delete;

private:
    QElapsedTimer   m_timer;        ///< USed to time the duration of the frame
    QPaintDevice&   m_paintable;    ///< Used to paint the debug info
    DebugDrawable&  m_drawable;     ///< Widget being recorded
    };


} // namespace utils::draw