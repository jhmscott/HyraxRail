/**
 * @file        common/swipeview.hpp
 * @brief       Widget that displays a stack of widgets that can be swiped between
 * @author      Justin Scott
 * @date        2026-10-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <common.hpp>

#include <utils/velocity.hpp>

#include <QEventPoint>
#include <QStackedWidget>
#include <QTouchEvent>


namespace ui::common
{
//////////////////////////////////////////////////////////////////////////////
/// Widget that displays a stack of widgets that can be swiped between
///
//////////////////////////////////////////////////////////////////////////////
class SwipeView : public QStackedWidget
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    explicit SwipeView (QWidget* parent);

signals:
    //////////////////////////////////////////////////////////////////////////////
    /// Signals the snap animation has completed
    ///
    /// @param[in]  idx     Index of widget snapped to
    ///
    //////////////////////////////////////////////////////////////////////////////
    void snappedTo (int idx);

protected:
    //////////////////////////////////////////////////////////////////////////////
    /// Handle an event
    ///
    /// @param[in]  event       Event to handle
    ///
    /// @return     True if the event was recognized
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual bool event (QEvent* event) override;

    //////////////////////////////////////////////////////////////////////////////
    /// Process a mouse wheel event
    ///
    /// @param[in]  event       Wheel event
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void wheelEvent (QWheelEvent* event) override;
private:
    utils::VelocityCalculator   m_velCalc;                      ///< Keeps track of points to calculate velocity
    QPointF                     m_trackpadDelta     = { 0, 0 }; ///< Current touchpad swipe distance
    int                         m_goingToPage       = -1;       ///< Set while the snap animation is in progress
                                                                ///  -1 when not
    bool                        m_swipeInProgress   = false;    ///< True if the user is currently swiping
    bool                        m_blockScroll       = false;    ///< (Touchpad only) Set when the swipe limit has been reached,
                                                                ///  and further scroll events should be ignored

    // Touchpad horizontal swipe gesture

    //////////////////////////////////////////////////////////////////////////////
    /// Start a touchpad swipe operation
    ///
    /// @param[in]  delta       Initial swipe distance in pixels
    ///
    //////////////////////////////////////////////////////////////////////////////
    void beginTouchpadSwipe (const QPointF& delta);

    //////////////////////////////////////////////////////////////////////////////
    /// Update the touchpad swipe state
    ///
    /// @param[in]  delta       Distance the touchpad has swiped in this update
    ///
    //////////////////////////////////////////////////////////////////////////////
    void updateTouchpadSwipe (const QPointF& delta);

    //////////////////////////////////////////////////////////////////////////////
    /// End a touchpad swipe
    ///
    /// @param[in]  animate     True to animate the snap
    ///                         False to jump directly to the tab
    ///
    //////////////////////////////////////////////////////////////////////////////
    void endTouchpadSwipe (bool animate);

    // Touch screen handling

    //////////////////////////////////////////////////////////////////////////////
    /// Handle a touch event
    ///
    /// @param[in]  event       Touch screen event
    ///
    //////////////////////////////////////////////////////////////////////////////
    void touchEvent (QTouchEvent* touch);

    //////////////////////////////////////////////////////////////////////////////
    /// Update the touch state with a new event point
    ///
    /// @param[in]  point       New touch point
    ///
    /// @return     true if a swipe is starting or in progress
    ///
    //////////////////////////////////////////////////////////////////////////////
    bool updateTouch (const QEventPoint& point);

    // Swipe state operations

    //////////////////////////////////////////////////////////////////////////////
    /// Begin a swipe operation
    ///
    //////////////////////////////////////////////////////////////////////////////
    void beginSwipe ();

    //////////////////////////////////////////////////////////////////////////////
    /// Update the widget to match how far the user has swiped
    ///
    /// @param[in]  dist        Distance the user has swiped
    ///
    //////////////////////////////////////////////////////////////////////////////
    void doSwipe (qreal dist);

    //////////////////////////////////////////////////////////////////////////////
    /// Call when the user has ended the swipe operation; aka released their finger
    ///
    /// @param[in]  point       Final touch point
    /// @param[in]  animate     True to animate the snap
    ///                         False to jump immediatly to the new tab
    ///
    //////////////////////////////////////////////////////////////////////////////
    void endSwipe (qreal dist, bool animate = true);

    //////////////////////////////////////////////////////////////////////////////
    /// Cancel a swipe operation
    ///
    //////////////////////////////////////////////////////////////////////////////
    void cancelSwipe ();

    // Snap animation operations

    //////////////////////////////////////////////////////////////////////////////
    /// Start the "snap" animation after the swipe
    ///
    /// @param[in]  idx     Index of widget to snap to
    ///
    //////////////////////////////////////////////////////////////////////////////
    void beginAnimation (int idx);

    //////////////////////////////////////////////////////////////////////////////
    /// Called when the "snap" animation finishes to reset the widget state
    ///
    //////////////////////////////////////////////////////////////////////////////
    void endAnimation ();

    };
} // namespace ui::common
