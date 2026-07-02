/**
 * @file        utils/draw.cpp
 * @brief       Utilities for 2D drawing/painting
 * @author      Justin Scott
 * @date        2026-06-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/draw.hpp>

#include <QPainterPath>

namespace utils::draw
{
static constexpr size_t FRAME_INFO_QUEUE_SIZE = 120;    ///< Number of frames to calculate rolling average from


void polygon (QPainter& painter, utils::PolygonViewF poly)
    {
    QPainterPath path;

    for (const utils::ComplexPolygonF& sub : poly)
        {
        path.addPolygon (sub.exteriorRing);

        for (const QPolygonF& hole : sub.interiorRings)
            {
            QPainterPath innerPath;

            innerPath.addPolygon (hole);

            path -= innerPath;
            }
        }

    painter.drawPath (path);
    }
ScopedDebugDraw::ScopedDebugDraw (QPaintDevice& painter, DebugDrawable& drawable) :
    m_paintable (painter),
    m_drawable (drawable)
    {
    m_timer.start ();
    }

ScopedDebugDraw::~ScopedDebugDraw ()
    {
    auto drawTime = m_timer.durationElapsed ();

    m_drawable.m_frames.emplace_back (drawTime);
    m_drawable.debugDraw (m_paintable);
    }

void DebugDrawable::debugDraw (QPaintDevice& paintable)
    {
    auto& [ time, _ ]  = m_frames.front ();

    double fps = (m_frames.size () * std::chrono::system_clock::period::den) /
                  // guard against division by 0
                  std::max ((std::chrono::system_clock::now () - time).count (), 1LL);
    double avgTime = 0.0;

    for (auto& [_, duration] : m_frames)
        {
        avgTime += duration.count ();
        }

    avgTime /= m_frames.size () * 1000.0 * 1000.0;

    QPainter painter{ &paintable };

    painter.drawText (painter.window (),
                      Qt::AlignRight | Qt::AlignTop,
                      QString::asprintf ("Frame Rate : %.1lf fps\nFrame Time : %.2lf ms",
                                         fps,
                                         avgTime));

    if (m_frames.size () > FRAME_INFO_QUEUE_SIZE)
        {
        m_frames.pop_front ();
        }
    }
}