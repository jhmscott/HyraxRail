/**
 * @file        testutils/mousespy.hpp
 * @brief       Class for detecting and collecting mouse events
 * @author      Justin Scott
 * @date        2026-10-31
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <common.hpp>

#include <QMouseEvent>
#include <QWidget>

namespace testutils
{

///////////////////////////////////////////////////////////////////////////////
/// Class for detecting and collecting mouse events
///
/// @remarks    Implemented with a std::list, because MouseEvents are not movable
///
///////////////////////////////////////////////////////////////////////////////
class MouseSpy : public QWidget, public std::list<QMouseEvent>
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit MouseSpy (QWidget* parent = NULL) :
        QWidget (parent)
        { setMouseTracking (true); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the number of recorded mouse events
    ///
    /// @return     Number of mouse events
    ///
    /// @remarks    Named count() to not conflict with QWidget::size()
    /// @remarks    Since we are using a std::list, O(N) not O(1)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t count () const { return std::distance (begin (), end ()); }
private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Store a mouse event
    ///
    /// @param[in]  event       Event to store
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void storeEvent (QMouseEvent* event)
        {
        emplace_back (event->type (),
                      event->position (),
                      event->globalPosition (),
                      event->button (),
                      event->buttons (),
                      event->modifiers (),
                      static_cast<const QPointingDevice*> (
                          event->device ()));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a mouse press event
    ///
    /// @param[in]  event       Mouse press event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void mousePressEvent (QMouseEvent* event) override { storeEvent (event); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a mouse release event
    ///
    /// @param[in]  event       Mouse release event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void mouseReleaseEvent (QMouseEvent* event) override { storeEvent (event); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a mouse double click event
    ///
    /// @param[in]  event       Mouse double click event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void mouseDoubleClickEvent (QMouseEvent* event) override { storeEvent (event); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a mouse move event
    ///
    /// @param[in]  event       Mouse move event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void mouseMoveEvent (QMouseEvent* event) override { storeEvent (event); }
    };


} // namespace testutils
