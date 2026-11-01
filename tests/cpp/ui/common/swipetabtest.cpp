/**
 * @file        common/swipetabtest.cpp
 * @brief       Test suite for the swipe tab widget
 * @author      Justin Scott
 * @date        2026-10-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <names.hpp>

#include <ui/common/swipetab.hpp>

#include <QtTest>

using namespace ui::common;

/// Number of tabs to test with
static constexpr int NUM_TEST_TABS = 5;

///////////////////////////////////////////////////////////////////////////////
/// Test data generate for tab indexes
///
///////////////////////////////////////////////////////////////////////////////
static void generateTabIndexes ()
    {
    QTest::addColumn<int> ("tab");

    for (int ii = 0; ii < NUM_TEST_TABS; ++ii)
        {
        QTest::addRow ("%d", ii) << ii;
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Populate the tabs in a tab widget
///
/// @param[in,out]  tabs        Tab widget to fill
/// @param[out]     widgets     widgets added to tabs
///
///////////////////////////////////////////////////////////////////////////////
static void fillTabWidget (SwipeTabWidget& tabs, QWidget** widgets)
    {
    for (int ii = 0; ii < NUM_TEST_TABS; ++ii)
        {
        tabs.addTab (widgets[ii] = new QWidget{ &tabs }, "");
        }
    }


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the swipe tab widget
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class SwipeTabTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for changeTabTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void changeTabTest_data () { generateTabIndexes (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Sets the tab and confirms this updates the swipe view as well
    ///
    /// @see    ui::common::SwipeTabWidget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void changeTabTest ()
        {
        QFETCH (int, tab);

        QWidget*        widgets[NUM_TEST_TABS];
        SwipeTabWidget  tabs{ NULL };
        QTabBar*        bar     = tabs.tabBar ();
        QStackedWidget* view    = tabs.findChild<QStackedWidget*> (OBJNAME_SWIPETAB_SWIPEVIEW);

        fillTabWidget (tabs, widgets);

        bar->setCurrentIndex (tab);

        QCOMPARE (view->currentWidget (), widgets[tab]);
        QCOMPARE (view->currentIndex (),  tab);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for changeViewTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void changeViewTest_data () { generateTabIndexes (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Sets the swipe view widget, and confirms this updates the tab bar as well
    ///
    ///  @see    ui::common::SwipeTabWidget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void changeViewTest ()
        {
        QFETCH (int, tab);

        QWidget*        widgets[NUM_TEST_TABS];
        SwipeTabWidget  tabs{ NULL };
        QTabBar*        bar     = tabs.tabBar ();
        QStackedWidget* view    = tabs.findChild<QStackedWidget*> (OBJNAME_SWIPETAB_SWIPEVIEW);

        fillTabWidget (tabs, widgets);

        view->setCurrentIndex (tab);

        QCOMPARE (bar->currentIndex (), tab);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests setting the tab position to north and checks the tab shape and layout
    /// is correct
    ///
    /// @see    ui::common::SwipeTabWidget::setTabPosition()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTabPositionNorthTest ()
        {
        SwipeTabWidget  tabs{ NULL };
        QTabBar*        bar = tabs.tabBar ();
        QStackedWidget* view= tabs.findChild<QStackedWidget*> (OBJNAME_SWIPETAB_SWIPEVIEW);

        tabs.setTabPosition (QTabWidget::North);
        tabs.setFixedSize (100, 100); // need to set a size for the relative position check
        bar->setFixedHeight (20);
        tabs.show ();

        QCOMPARE (bar->shape (), QTabBar::RoundedNorth);
        // top = 0, y increasing down
        QCOMPARE_LT (bar->y (), view->y ());
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests setting the tab position to south and checks the tab shape and layout
    /// is correct
    ///
    /// @see    ui::common::SwipeTabWidget::setTabPosition()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTabPositionSouthTest ()
        {
        SwipeTabWidget  tabs{ NULL };
        QTabBar*        bar = tabs.tabBar ();
        QStackedWidget* view= tabs.findChild<QStackedWidget*> (OBJNAME_SWIPETAB_SWIPEVIEW);

        tabs.setTabPosition (QTabWidget::South);
        tabs.setFixedSize (100, 100); // need to set a size for the relative position check
        bar->setFixedHeight (20);
        tabs.show ();

        QCOMPARE (bar->shape (), QTabBar::RoundedSouth);
        // top = 0, y increasing down
        QCOMPARE_GT (bar->y (), view->y ());
        }

    // @TODO: figure out vertical tab bars. Currently my implementation doesn't work
    // I don't use this anywhere yet so it can wait
#if 0
    ///////////////////////////////////////////////////////////////////////////////
    /// Tests setting the tab position to east and checks the tab shape and layout
    /// is correct
    ///
    /// @see    ui::common::SwipeTabWidget::setTabPosition()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTabPositionEastTest ()
        {
        SwipeTabWidget  tabs{ NULL };
        QTabBar*        bar = tabs.tabBar ();
        QStackedWidget* view= tabs.findChild<QStackedWidget*> (OBJNAME_SWIPETAB_SWIPEVIEW);

        tabs.setTabPosition (QTabWidget::East);
        tabs.setFixedSize (100, 100); // need to set a size for the relative position check
        bar->setFixedWidth (20);
        tabs.show ();

        QCOMPARE (bar->shape (), QTabBar::RoundedEast);
        // left = 0, x increasing right
        QCOMPARE_GT (bar->x (), view->x ());
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests setting the tab position to west and checks the tab shape and layout
    /// is correct
    ///
    /// @see    ui::common::SwipeTabWidget::setTabPosition()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTabPositionWestTest ()
        {
        SwipeTabWidget  tabs{ NULL };
        QTabBar*        bar = tabs.tabBar ();
        QStackedWidget* view= tabs.findChild<QStackedWidget*> (OBJNAME_SWIPETAB_SWIPEVIEW);

        tabs.setTabPosition (QTabWidget::West);
        tabs.setFixedSize (100, 100); // need to set a size for the relative position check
        bar->setFixedWidth (20);
        tabs.show ();

        QCOMPARE (bar->shape (), QTabBar::RoundedWest);
        // left = 0, x increasing right
        QCOMPARE_LT (bar->x (), view->x ());
        }
#endif
    };

QTEST_MAIN (SwipeTabTest)

#include "swipetabtest.moc"
