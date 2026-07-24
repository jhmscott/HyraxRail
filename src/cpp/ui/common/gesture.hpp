/**
 * @file        common/gesture.hpp
 * @brief       Gesture recognition utilities
 * @author      Justin Scott
 * @date        2026-07-24
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QContextMenuEvent>
#include <QGestureEvent>
#include <QGraphicsItem>
#include <QWidget>

namespace ui::common
{

///////////////////////////////////////////////////////////////////////////////
/// Adds the functionality to open the context menu when you tap and hold the
/// widget or graphics object
///
/// @tparam     Widget      Type derived from QWidget or QGraphicsObject
///
///////////////////////////////////////////////////////////////////////////////
template<class Widget>
class HoldToOpenMenu : public Widget
    {
public:
    static constexpr bool is_widget             = std::is_base_of_v<QWidget,        Widget>;
    static constexpr bool is_graphics_object    = std::is_base_of_v<QGraphicsObject,Widget>;

    static_assert (is_widget || is_graphics_object,
                   "Must inherit from QWidget or QGraphicsObject");

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @tparam     Args...     Argument types
    ///
    /// @param[in]  args...     Constructor arguments
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class... Args>
    HoldToOpenMenu (Args... args) :
        Widget (args...)
        { Widget::grabGesture (Qt::TapAndHoldGesture); }

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// Handle an event
    ///
    /// @param[in]  event       Event to handle
    ///
    /// @return     true if handled
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool event (QEvent* event) override
        {
        bool rc;

        switch (event->type ())
            {
            case QEvent::Gesture:
                {
                QGesture* gesture;

                if (NULL != (gesture =
                                static_cast<QGestureEvent*> (event)->
                                    gesture (Qt::TapAndHoldGesture)))
                    {
                    QTapAndHoldGesture* hold = static_cast<QTapAndHoldGesture*> (gesture);

                    openContextMenu (hold->position ().toPoint ());

                    rc = true;
                    break;
                    }
                else
                    {
                    [[fallthrough]];
                    }
                }
            default:
                {
                rc = Widget::event (event);
                break;
                }
            }

        return rc;
        }

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Open the context menu at a given location
    ///
    /// @param[in]  pt      Point to open the context menu at (local coordinates)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void openContextMenu (const QPoint& pt)
        {
        QContextMenuEvent   event{ QContextMenuEvent::Mouse,
                                   pt,
                                   localToGlobalPoint (pt) };

        QCoreApplication::sendEvent (this, &event);
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Map a point from local to global coordinates
    ///
    /// @param[in]  pt      Point to map (local coordinates)
    ///
    /// @return     pt in global coordinates
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QPoint localToGlobalPoint (const QPoint& pt) const
        {
        QPoint global;

        if constexpr (is_widget)
            {
            global = Widget::mapToGlobal (pt);
            }
        else if (NULL == Widget::scene ())
            {
            qDebug () << "Graphics object missing scene";
            }
        else if (Widget::scene ()->views.isEmpty ())
            {
            qDebug () << "Graphics scene missing views";
            }
        else
            {
            QGraphicsView*  view        = Widget::scene ()->views ().first ();
            QPoint          scenePoint  = Widget::mapToScene (pt);

            global = view->mapToGlobal (view->mapFromScene (scenePoint));
            }

        return global;
        }
    };

} // namespace ui::common