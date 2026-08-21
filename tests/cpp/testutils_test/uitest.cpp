/**
 * @file        testutils_test/uitest.hpp
 * @brief       Test suite for UI test utilities
 * @author      Justin Scott
 * @date        2026-08-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <testutils/ui.hpp>

#include <QtTest>


static QString MENU_ITEMS[] =
    {
    "Test Item",
    "Fake Item",
    "Real Item (Just Kidding)"
    };

///////////////////////////////////////////////////////////////////////////////
/// Validate that a menu has the correct text
///
/// @param[in]  menu        Menu item to validate
///
///////////////////////////////////////////////////////////////////////////////
void validateMenu (const QMenu& menu)
    {
    QList<QAction*> actions = menu.actions ();

    QCOMPARE (actions.size (), std::size (MENU_ITEMS));

    for (int ii = 0; ii < actions.size (); ++ii)
        {
        QCOMPARE (actions[ii]->text (), MENU_ITEMS[ii]);
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for UI test utilities
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class UiTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for testMenuItemTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void testMenuItemTest_data ()
        {
        QTest::addColumn<int> ("menuId");

        for (int ii = 0; ii < static_cast<int> (std::size (MENU_ITEMS)); ++ii)
            {
            QTest::addRow ("ID %d", ii) << ii;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the testMenuItem() function
    ///
    /// @see    testutils::testMenuItem()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void testMenuItemTest ()
        {
        QFETCH (int, menuId);

        QWidget menuTest;

        for (const QString& text : MENU_ITEMS)
            {
            menuTest.addAction (text);
            }

        menuTest.setContextMenuPolicy (Qt::ActionsContextMenu);
        menuTest.show ();

        QList<QAction*>                             actions = menuTest.actions ();
        std::vector<std::unique_ptr<QSignalSpy>>    spies;

        spies.reserve (actions.size ());

        std::transform (actions.begin (),
                        actions.end (),
                        std::back_inserter (spies),
                        [] (QAction* action) -> std::unique_ptr<QSignalSpy>
                        { return std::make_unique<QSignalSpy> (action, &QAction::triggered); });

        bool isCalled = false;

        testutils::testMenuItem (menuTest,
                                 menuId,
                                 validateMenu,
                                 [&isCalled] () -> void { isCalled = true; });

        QVERIFY (isCalled);

        for (int ii = 0; ii < actions.size (); ++ii)
            {
            // Only the selected item should have been triggered
            if (menuId == ii)
                {
                QCOMPARE (spies[ii]->size (), 1);
                }
            else
                {
                QCOMPARE (spies[ii]->size (), 0);
                }
            }
        }
    };

QTEST_MAIN (UiTest)

#include "uitest.moc"
