/**
 * @file        common/swipeview.cpp
 * @brief       Widget that displays a stack of widgets that can be swiped between
 * @author      Justin Scott
 * @date        2026-10-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/swipeview.hpp>
#include <ui/common/utils.hpp>

#include <utils/math.hpp>

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

namespace ui::common
{
/// Minimum distance, in pixels, for the touch point to move before recognizing a swipe
static qreal    MIN_SWIPE_DIST  = 5.0;
/// Inertia of a swipe. Applies velocity of swipe to determine snapping
static qreal    SWIPE_INERTIA   = 0.2;
/// Speed of the snap animation in milliseconds
static int      ANIMATION_SPEED = 100;

/// Number of points to use to calculate the average velocity
static size_t   NUM_VELOCITY_PTS= 20;
/// Decay factor used in calculating the velocity
static double   VELOCITY_DECAY  = 0.9;


//////////////////////////////////////////////////////////////////////////////
/// Get the distance a point has swiped from it's origin
///
/// @param[in]  point       Point to calculate swipe distance for
///
/// @return     Swipe distance in pixels
///
/// @remarks    Swipe is strictly in the x direction
///
//////////////////////////////////////////////////////////////////////////////
static qreal swipeDistance (const QEventPoint& point)
    {
    return point.position ().x () - point.pressPosition ().x ();
    }

//////////////////////////////////////////////////////////////////////////////
/// Show the next/previous widget
///
/// @param[in,out]  stack       Widget stack
/// @param[in]      next        True to show next widget. False to show previous
///
/// @remarks        Does nothing if next/previous widget does not exist
///
//////////////////////////////////////////////////////////////////////////////
static void showWidget (QStackedWidget& stack, bool next)
    {
    int         idx     = stack.currentIndex () + (next ? 1 : -1);
    QWidget*    wdgt    = stack.widget (idx);

    if (NULL != wdgt)
        {
        QRect frame     = stack.frameRect ();
        int   xPos      = stack.currentWidget ()->x () +
                         (next ? frame.width () : -frame.width ());

        wdgt->setGeometry (0, 0, frame.width (), frame.height ());
        wdgt->show ();
        wdgt->raise ();
        wdgt->move (QPoint{ xPos, 0 });
        }
    }

//////////////////////////////////////////////////////////////////////////////
/// Move a widget according to a swipe distance
///
/// @param[in,out]  stack   Widget stack
/// @param[in]      idx     Index of widget to move
/// @param[in]      dist    Swipe distance to move widget by
///
/// @remarks        Does nothing if widget[idx] does not exist
///
//////////////////////////////////////////////////////////////////////////////
static void swipeWidget (QStackedWidget& stack, int idx, qreal dist)
    {
    QWidget* wdgt = stack.widget (idx);

    if (NULL != wdgt)
        {
        wdgt->move (QPoint{ utils::math::roundToInt (dist), 0});
        }
    }

//////////////////////////////////////////////////////////////////////////////
/// Hide a widget in a stack
///
/// @param[in,out]  stack   Widget stack
/// @param[in]      idx     Index of widget to hide
///
/// @remarks        Does nothing if widget[idx] does not exist
///
//////////////////////////////////////////////////////////////////////////////
static void hideWidget (QStackedWidget& stack, int idx)
    {
    QWidget* wdgt = stack.widget (idx);

    if (NULL != wdgt)
        {
        wdgt->hide ();
        }
    }

//////////////////////////////////////////////////////////////////////////////
/// Create a snap animation for a widget in a stack
///
/// @param[in,out]  stack   Widget stack
/// @param[in]      idx     Index of widget to hide
/// @param[in]      final   Final position to snap the widget to
/// @param[in,out]  group   Animation group to add to
///
//////////////////////////////////////////////////////////////////////////////
static void createAnimation (QStackedWidget&            stack,
                             int                        idx,
                             const QPoint&              final,
                             QParallelAnimationGroup*   group)
    {
    QPropertyAnimation* anim = NULL;
    QWidget*            wdgt = stack.widget (idx);

    if (NULL != wdgt)
        {
        anim = new QPropertyAnimation (wdgt, "pos");

        anim->setDuration (ANIMATION_SPEED);
        anim->setEasingCurve (QEasingCurve::OutQuart);
        anim->setStartValue (wdgt->pos ());
        anim->setEndValue (final);

        group->addAnimation (anim);
        }
    }

//////////////////////////////////////////////////////////////////////////////
/// Create a fake mouse event for a touch event
///
/// @param[in]  touchEvent      Event to generate mouse event for
///
//////////////////////////////////////////////////////////////////////////////
static void fakeMouseEvent (const QTouchEvent& touchEvent)
    {
    QEvent::Type    eventType   = QEvent::None;
    QEventPoint     touchPoint  = touchEvent.points ().first ();

    switch (touchEvent.type ())
        {
        case QEvent::TouchBegin:
            {
            eventType = QEvent::MouseButtonPress;
            break;
            }
        case QEvent::TouchUpdate:
            {
            eventType = QEvent::MouseMove;
            break;
            }
        case QEvent::TouchEnd:
            {
            eventType = QEvent::MouseButtonRelease;
            break;
            }
        default:
            {
            break;
            }
        }

    QPoint      global  = touchPoint.globalPosition ().toPoint ();
    QWidget*    target  = QApplication::widgetAt (global);

    if (NULL != target)
        {
        QMouseEvent mouseEvent{ eventType,
                                target->mapFromGlobal (global),
                                global,
                                Qt::LeftButton,
                                Qt::LeftButton,
                                touchEvent.modifiers () };

        QApplication::sendEvent (target, &mouseEvent);
        }
    }

SwipeView::SwipeView (QWidget* parent) :
    QStackedWidget (parent),
    m_velCalc (NUM_VELOCITY_PTS, VELOCITY_DECAY)
    {
    setAttribute (Qt::WA_AcceptTouchEvents);
    setContentsMargins (0, 0, 0, 0);
    }

bool SwipeView::event (QEvent* event)
    {
    bool rc = false;

    switch (event->type ())
        {
        case QTouchEvent::TouchBegin:
        case QTouchEvent::TouchUpdate:
        case QTouchEvent::TouchEnd:
        case QTouchEvent::TouchCancel:
            {
            if (-1 == m_goingToPage) // Block touch events during snap animation
                {
                touchEvent (static_cast<QTouchEvent*> (event));
                rc = true;
                break;
                }
            [[fallthrough]];
            }
        default:
            {
            rc = QStackedWidget::event (event);
            break;
            }
        }

    return rc;
    }

void SwipeView::touchEvent (QTouchEvent* touch)
    {
    switch (touch->type ())
        {
        case QTouchEvent::TouchBegin:
            {
            m_velCalc.clear ();
            [[fallthrough]];
            }
        case QTouchEvent::TouchUpdate:
            {
            if (not updateTouch (touch->point (0)))
                {
                // Convert to mouse event if we aren't in a swipe operation
                fakeMouseEvent (*touch);
                }
            break;
            }
        case QTouchEvent::TouchEnd:
            {
            if (m_swipeInProgress)
                {
                endSwipe (touch->point (0));
                }
            else
                {
                fakeMouseEvent (*touch);
                }
            break;
            }
        case QTouchEvent::TouchCancel:
            {
            if (m_swipeInProgress)
                {
                cancelSwipe ();
                }
            break;
            }
        default:
            {
            break;
            }
        }

    touch->accept ();
    }

bool SwipeView::updateTouch (const QEventPoint& point)
    {
    bool    handled = false;
    qreal   dist    = swipeDistance (point);

    if (m_swipeInProgress)
        {
        doSwipe (dist);
        handled = true;
        }
    else if (MIN_SWIPE_DIST < abs (dist))
        {
        beginSwipe ();
        doSwipe (dist);
        handled = true;
        }

    if (QEventPoint::Stationary == point.state ())
        {
        // Stationary meaning full stop, reset velocity to 0
        m_velCalc.clear ();
        }
    else
        {
        m_velCalc.addPoint (point.lastPosition (), point.lastTimestamp ());
        }

    return handled;
    }

void SwipeView::beginSwipe ()
    {
    showWidget (*this, true);
    showWidget (*this, false);

    m_swipeInProgress = true;
    }

void SwipeView::doSwipe (qreal dist)
    {
    int idx     = currentIndex ();
    int width   = frameRect ().width ();

    swipeWidget (*this, idx - 1,   -width + dist);
    swipeWidget (*this, idx,        dist);
    swipeWidget (*this, idx + 1,    width + dist);
    }

void SwipeView::endSwipe (const QEventPoint& point)
    {
    // So QEventPoint::velocity() gives unusable values
    // My relatively naive implementation behaves pretty well and I can tweak
    // the constants as needed
    qreal   vel  = m_velCalc.getVelocity ().x ();
    qreal   dist = swipeDistance (point);
    int     idx;

    // qDebug () << "Dist: " << dist << "Vel: " << vel;

    dist += vel * SWIPE_INERTIA;

    if (abs (dist) < currentWidget ()->width () / 2)
        {
        idx = currentIndex ();
        }
    else if (dist > 0)
        {
        idx = currentIndex () - 1;
        }
    else // dist < 0
        {
        idx = currentIndex () + 1;
        }

    // Domain check, snap to current if the end has been reached
    if (idx < 0 || idx >= count ())
        {
        idx = currentIndex ();
        }

    beginAnimation (idx);
    }

void SwipeView::cancelSwipe ()
    {
    // snap back to the current widget
    beginAnimation (currentIndex ());
    }

void SwipeView::beginAnimation (int idx)
    {
    QParallelAnimationGroup*    group   = new QParallelAnimationGroup;
    int                         width   = frameRect ().width ();

    createAnimation (*this, idx - 1, QPoint{ -width, 0 },   group);
    createAnimation (*this, idx,     QPoint{ 0, 0 },        group);
    createAnimation (*this, idx + 1, QPoint{ width, 0 },    group);

    connect (group,
            &QParallelAnimationGroup::finished,
             this,
            &SwipeView::endAnimation);

    m_goingToPage = idx;

    group->start (QAbstractAnimation::DeleteWhenStopped);

    m_swipeInProgress = false;
    }

void SwipeView::endAnimation ()
    {
    for (int ii = 0; ii < count (); ++ii)
        {
        if (m_goingToPage != ii)
            {
            hideWidget (*this, ii);
            }
        }

    setCurrentIndex (m_goingToPage);
    emit snappedTo (m_goingToPage);

    m_goingToPage = -1;
    }

} // namespace ui::common
