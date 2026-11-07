/**
 * @file        layout/buttontest.cpp
 * @brief       Test suite for the layout button
 * @author      Justin Scott
 * @date        2026-11-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <testutils/ui.hpp>

#include <ui/common/layout/button.hpp>

#include <QtTest>

using namespace ui::common;

///////////////////////////////////////////////////////////////////////////////
/// Poor man's mocking, fakeit doesn't work with multiple inheritance, which
/// all QWidget derived classes have (QObject and QPaintDevice). There's only
/// two functions, so the additional boiler plate isn't too bad
///
///////////////////////////////////////////////////////////////////////////////
class LayoutButtonMockish : public LayoutButton
    {
    friend class LayoutButtonTest;
public:
    using LayoutButton::LayoutButton;

    int editCalls   = 0; ///< Number of times editComponent() was called
    int removeCalls = 0; ///< Number of times removeComponent() was called

private:
    virtual void editComponent () override { ++editCalls; }

    virtual void removeComponent () override { ++removeCalls; }
    };

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the UI layout button
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class LayoutButtonTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test a button with no menu
    ///
    /// @see    ui::common::LayoutButton
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void noMenuTest ()
        {
        LayoutButtonMockish btn{ false, NULL };

        QTimer::singleShot (10,
        [&] () -> void
            {
            QMenu* menu = ui::common::getTopLevelWidget<QMenu> ();

            QCOMPARE (menu, NULL);
            });

        QTest::mouseClick (&btn, Qt::RightButton);

        QTest::qWait (20);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for hasMenuTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void hasMenuTest_data ()
        {
        QTest::addColumn<int> ("id");

        QTest::newRow ("delete") << 0;
        QTest::newRow ("edit")   << 1;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test a layout button's menu
    ///
    /// @see    ui::common::LayoutButton
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void hasMenuTest ()
        {
        QFETCH (int, id);

        LayoutButtonMockish btn{ true, NULL };

        auto validateMenu =
            [] (const QMenu& menu) -> void
            {
            auto actions = menu.actions ();

            QCOMPARE (actions.at (0)->text (), "Delete");
            QCOMPARE (actions.at (1)->text (), "Edit");
            };

        btn.show ();

        testutils::testMenuItem (btn,
                                 id,
                                 validateMenu,
                                 [] () -> void {});

        if (0 == id)
            {
            QCOMPARE (btn.editCalls, 0);
            QCOMPARE (btn.removeCalls, 1);
            }
        else
            {
            QCOMPARE (btn.editCalls, 1);
            QCOMPARE (btn.removeCalls, 0);
            }
        }
    };

QTEST_MAIN (LayoutButtonTest)

#include "buttontest.moc"
