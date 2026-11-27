/**
 * @file        common/swipeviewtest.cpp
 * @brief       Test suite for the swipe view widget
 * @author      Justin Scott
 * @date        2026-10-31
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <testutils/mousespy.hpp>

#include <ui/common/swipeview.hpp>

#include <QtTest>

using namespace ui::common;


using doSwipeFunc   = void (SwipeView&     view,
                            const QPoint&  start,
                            const QPoint&  end,
                            int            time);

/// Number of tabs to test with
static constexpr int    NUM_TEST_VIEWS  = 5;
/// Size of SwipeView
static constexpr QSize  VIEW_SIZE       = { 100, 100 };

///////////////////////////////////////////////////////////////////////////////
/// Setup a SwipeView widget
///
/// @param[in,out]  view        View to setup
/// @param[in]      start       Widget to start on
///
///////////////////////////////////////////////////////////////////////////////
static void initView (SwipeView& view, int start)
    {
    for (int ii = 0; ii < NUM_TEST_VIEWS; ++ii)
        {
        view.addWidget (new QWidget{ &view });
        }

    view.setFixedSize (VIEW_SIZE);
    view.setCurrentIndex (start);
    view.show ();
    }

///////////////////////////////////////////////////////////////////////////////
/// Swipe a widget with a touch screen
///
/// @param[in,out]  view    Widget to swipe
/// @param[in]      start   Starting point
/// @param[in]      end     End point
/// @param[in]      time    (optional) Time to perform swipe in milliseconds
///                         -1 results in an instant swipe (default)
///
///////////////////////////////////////////////////////////////////////////////
static void doTouchScreenSwipe (SwipeView&     view,
                                const QPoint&  start,
                                const QPoint&  end,
                                int            time = -1)
    {
    QPointingDevice*    dev = QTest::createTouchDevice ();
    auto                seq = QTest::touchEvent (&view, dev);

    seq.press (0, start).commit ();

    if (time > 0)
        {
        QTest::qWait (time);
        }

    seq.move (0, end).commit ();

    if (time < 0)
        {
        seq.stationary (0).commit ();
        }

    seq.release (0, end).commit ();
    }

///////////////////////////////////////////////////////////////////////////////
/// Swipe a widget with a touchpad
///
/// @param[in,out]  view    Widget to swipe
/// @param[in]      start   Starting point
/// @param[in]      end     End point
/// @param[in]      time    (optional) Time to perform swipe in milliseconds
///                         -1 results in an instant swipe (default)
///
///////////////////////////////////////////////////////////////////////////////
static void doTouchPadSwipe (SwipeView&     view,
                             const QPoint&  start,
                             const QPoint&  end,
                             int            time = -1)
    {
    QWindow* window = view.windowHandle ();

    if (NULL == window)
        {
        window = q_check_ptr (view.topLevelWidget ())->windowHandle ();
        Q_CHECK_PTR (window);
        }

    QTest::wheelEvent (window,
                       start,
                       { 1, 1 },
                       start - end, // Does not set inverted to true, so we need to invert it here
                       Qt::NoModifier,
                       Qt::ScrollBegin);

    QTest::wheelEvent (window,
                       end,
                       { 1, 1 },
                       { 1, 1 },
                       Qt::NoModifier,
                       Qt::ScrollEnd);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test a swipe gesture
///
/// @param[in]  start       Starting point
/// @param[in]  end         End point
/// @param[in]  before      Widget to start on
/// @param[in]  after       Expected widget to end on
/// @param[in]  doSwipe     Function to actually swipe the widget
/// @param[in]  time        Time to perform swipe in milliseconds
///                         -1 results in an instant swipe (default)
///
///////////////////////////////////////////////////////////////////////////////
static void testSwipe (const QPoint&    start,
                       const QPoint&    end,
                       int              before,
                       int              after,
                       doSwipeFunc*     doSwipe,
                       int              time)
    {
    SwipeView   view{ NULL };
    QSignalSpy  spy{ &view, &SwipeView::snappedTo };

    Q_ASSERT (doSwipe);

    initView (view, before);
    doSwipe (view, start, end, time);

    QVERIFY (spy.wait (200));

    QCOMPARE (view.currentIndex (), after);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test a swipe gesture
///
/// @param[in]  start       Starting point
/// @param[in]  end         End point
/// @param[in]  before      Widget to start on
/// @param[in]  after       Expected widget to end on
/// @param[in]  time        (optional) Time to perform swipe in milliseconds
///                         -1 results in an instant swipe (default)
///
///////////////////////////////////////////////////////////////////////////////
void testTouchScreenSwipe (const QPoint&    start,
                           const QPoint&    end,
                           int              before,
                           int              after,
                           int              time = -1)
    {
    testSwipe (start,
               end,
               before,
               after,
              &doTouchScreenSwipe,
               time);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test a swipe gesture with a touch pad
///
/// @param[in]  start       Starting point
/// @param[in]  end         End point
/// @param[in]  before      Widget to start on
/// @param[in]  after       Expected widget to end on
///
///////////////////////////////////////////////////////////////////////////////
void testTouchPadSwipe (const QPoint&    start,
                        const QPoint&    end,
                        int              before,
                        int              after)
    {
    testSwipe (start,
               end,
               before,
               after,
              &doTouchPadSwipe,
               -1);
    }

///////////////////////////////////////////////////////////////////////////////
/// Generate a range of widget indexes
///
/// @param[in]  start       Start index
/// @param[in]  end         End index
///
///////////////////////////////////////////////////////////////////////////////
static void generateRange (int start, int end)
    {
    QTest::addColumn<int> ("start");

    for (int ii = start; ii < end; ++ii)
        {
        QTest::addRow ("%d", ii) << ii;
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the swipe tab widget
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class SwipeViewTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for swipeNextTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipeNextTest_data () { generateRange (0, NUM_TEST_VIEWS - 1); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test swiping to the next widget
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipeNextTest ()
        {
        QFETCH (int, start);

        testTouchScreenSwipe (QPoint{ 81, 50 },
                              QPoint{ 30, 50 },
                              start,
                              start + 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for swipeNextTouchPadTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipeNextTouchPadTest_data () { generateRange (0, NUM_TEST_VIEWS - 1); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test swiping to the next widget with a touch pad
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipeNextTouchPadTest ()
        {
        QFETCH (int, start);

        testTouchPadSwipe (QPoint{ 81, 50 },
                           QPoint{ 30, 50 },
                           start,
                           start + 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for swipePreviousTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipePreviousTest_data () { generateRange (1, NUM_TEST_VIEWS); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test swiping to the previous widget
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipePreviousTest ()
        {
        QFETCH (int, start);

        testTouchScreenSwipe (QPoint{ 30, 50 },
                              QPoint{ 81, 50 },
                              start,
                              start - 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for swipePreviousTouchPadTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipePreviousTouchPadTest_data () { generateRange (1, NUM_TEST_VIEWS); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test swiping to the previous widget with a touch pad
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipePreviousTouchPadTest ()
        {
        QFETCH (int, start);

        testTouchPadSwipe (QPoint{ 30, 50 },
                           QPoint{ 81, 50 },
                           start,
                           start - 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for swipeNextSnapBackTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipeNextSnapBackTest_data () { generateRange (0, NUM_TEST_VIEWS); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Attempts to swipe to the next widget, but does not swipe far enough,
    /// resulting in a "snap" back to the current
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipeNextSnapBackTest ()
        {
        QFETCH (int, start);

        testTouchScreenSwipe (QPoint{ 79, 50 },
                              QPoint{ 30, 50 },
                              start,
                              start);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for swipeNextSnapBackTouchPadTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipeNextSnapBackTouchPadTest_data () { generateRange (0, NUM_TEST_VIEWS); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Attempts to swipe to the next widget with a touch pad, but does not swipe
    /// far enough,   resulting in a "snap" back to the current
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipeNextSnapBackTouchPadTest ()
        {
        QFETCH (int, start);

        testTouchPadSwipe (QPoint{ 79, 50 },
                           QPoint{ 30, 50 },
                           start,
                           start);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for swipePreviousSnapBackTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipePreviousSnapBackTest_data () { generateRange (0, NUM_TEST_VIEWS); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Attempts to swipe to the previous widget, but does not swipe far enough,
    /// resulting in a "snap" back to the current
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipePreviousSnapBackTest ()
        {
        QFETCH (int, start);

        testTouchScreenSwipe (QPoint{ 30, 50 },
                              QPoint{ 79, 50 },
                              start,
                              start);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for swipePreviousSnapBackTouchPadTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipePreviousSnapBackTouchPadTest_data () { generateRange (0, NUM_TEST_VIEWS); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Attempts to swipe to the previous widget with a touch pad, but does not
    /// swipe far enough, resulting in a "snap" back to the current
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipePreviousSnapBackTouchPadTest ()
        {
        QFETCH (int, start);

        testTouchPadSwipe (QPoint{ 30, 50 },
                           QPoint{ 79, 50 },
                           start,
                           start);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Attempt to swipe to the next widget, when already at the last. Should snap
    /// back to the current
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipeNextEndTest ()
        {
        testTouchScreenSwipe (QPoint{ 81, 50 },
                              QPoint{ 30, 50 },
                              NUM_TEST_VIEWS - 1,
                              NUM_TEST_VIEWS - 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Attempt to swipe to the next widget with a touch pad, when already at the
    /// last. Should snap back to the current
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipeNextEndTouchPadTest ()
        {
        testTouchPadSwipe (QPoint{ 81, 50 },
                           QPoint{ 30, 50 },
                           NUM_TEST_VIEWS - 1,
                           NUM_TEST_VIEWS - 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Attempt to swipe to the previous widget, when already at the first. Should snap
    /// back to the current
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipePreviousStartTest ()
        {
        testTouchScreenSwipe (QPoint{ 30, 50 },
                              QPoint{ 81, 50 },
                              0,
                              0);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Attempt to swipe to the previous widget with a touch pad, when already at
    /// the first. Should snap back to the current
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void swipePreviousStartTouchPadTest ()
        {
        testTouchPadSwipe (QPoint{ 30, 50 },
                           QPoint{ 81, 50 },
                           0,
                           0);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Attempt a small swipe, too small to be considered a true swipe. This should
    /// generate mouse events for the underlying widget
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void smallSwipeTest ()
        {
        SwipeView               view{ NULL };
        QSignalSpy              spy{ &view, &SwipeView::snappedTo };
        testutils::MouseSpy*    mouse = new testutils::MouseSpy{ &view };

        view.addWidget (mouse);
        initView (view, 0);

        // only moves one pixel in the x
        // Lots in the y tho
        QPoint start{ 50, 50 };
        QPoint end{ 51, 80 };

        doTouchScreenSwipe (view, start, end);

        // Check that a snap hasn't been emitted
        QVERIFY (not spy.wait (200));

        // Check the correct mouse events  gave been emitted
        QCOMPARE (mouse->count (), 3);

        auto first  = mouse->begin ();
        auto second = std::next (first);
        auto third  = std::next (second);

        QCOMPARE (first->position ().toPoint (), start);
        QCOMPARE (first->type (), QEvent::MouseButtonPress);

        QCOMPARE (second->position ().toPoint (), end);
        QCOMPARE (second->type (), QEvent::MouseMove);

        QCOMPARE (third->position ().toPoint (), end);
        QCOMPARE (third->type (), QEvent::MouseButtonRelease);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Attempt a small, fast swipe, to confirm the inertia system takes into account
    /// the velocity
    ///
    /// @see    ui::common::SwipeView
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void fastSwipeTest ()
        {
        testTouchScreenSwipe (QPoint{ 50, 50 },
                              QPoint{ 30, 50 },
                              0,
                              0,
                              50); // 50 ms
        }

    void momentumTouchPadSwipeTest ()
        {
        testSwipe (QPoint{ 50, 50 },
                   QPoint{ 30, 50 },
                   0,
                   1,
            [] (SwipeView&      view,
                const QPoint&   start,
                const QPoint&   end,
                int             time) -> void
            {
            QWindow* window = view.windowHandle ();

            if (NULL == window)
                {
                window = q_check_ptr (view.topLevelWidget ())->windowHandle ();
                Q_CHECK_PTR (window);
                }

            QTest::wheelEvent (window,
                               start,
                               { 1, 1 },
                               start - end, // Does not set inverted to true, so we need to invert it here
                               Qt::NoModifier,
                               Qt::ScrollBegin);

            QTest::wheelEvent (window,
                               end,
                               { 1, 1 },
                               (start - end) * 2,
                               Qt::NoModifier,
                               Qt::ScrollMomentum);

            QTest::wheelEvent (window,
                            end,
                            { 1, 1 },
                            { 1, 1 },
                            Qt::NoModifier,
                            Qt::ScrollEnd);
            },
            -1);
        }
    };

QTEST_MAIN (SwipeViewTest)

#include "swipeviewtest.moc"
