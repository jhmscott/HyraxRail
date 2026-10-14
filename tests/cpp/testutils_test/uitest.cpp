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
static void validateMenu (const QMenu& menu)
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

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for clickStandardButtonTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clickStandardButtonTest_data ()
        {
        QTest::addColumn<QDialogButtonBox::StandardButton> ("button");

        QTest::newRow ("Ok")            << QDialogButtonBox::Ok;
        QTest::newRow ("Save")          << QDialogButtonBox::Save;
        QTest::newRow ("Save All")      << QDialogButtonBox::SaveAll;
        QTest::newRow ("Open")          << QDialogButtonBox::Open;
        QTest::newRow ("Yes")           << QDialogButtonBox::Yes;
        QTest::newRow ("Yes to All")    << QDialogButtonBox::YesToAll;
        QTest::newRow ("No")            << QDialogButtonBox::No;
        QTest::newRow ("No to All")     << QDialogButtonBox::NoToAll;
        QTest::newRow ("Abort")         << QDialogButtonBox::Abort;
        QTest::newRow ("Retry")         << QDialogButtonBox::Retry;
        QTest::newRow ("Ignore")        << QDialogButtonBox::Ignore;
        QTest::newRow ("Close")         << QDialogButtonBox::Close;
        QTest::newRow ("Cancel")        << QDialogButtonBox::Cancel;
        QTest::newRow ("Discard")       << QDialogButtonBox::Discard;
        QTest::newRow ("Help")          << QDialogButtonBox::Help;
        QTest::newRow ("Apply")         << QDialogButtonBox::Apply;
        QTest::newRow ("Reset")         << QDialogButtonBox::Reset;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests clicking a standard dialog box button
    ///
    /// @see    testutils::clickStandardButton()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clickStandardButtonTest ()
        {
        QFETCH (QDialogButtonBox::StandardButton, button);

        QDialog dlg;

        auto* buttons   = new QDialogButtonBox{ QDialogButtonBox::Ok        |
                                                QDialogButtonBox::Save      |
                                                QDialogButtonBox::SaveAll   |
                                                QDialogButtonBox::Open      |
                                                QDialogButtonBox::Yes       |
                                                QDialogButtonBox::YesToAll  |
                                                QDialogButtonBox::No        |
                                                QDialogButtonBox::NoToAll   |
                                                QDialogButtonBox::Abort     |
                                                QDialogButtonBox::Retry     |
                                                QDialogButtonBox::Ignore    |
                                                QDialogButtonBox::Cancel    |
                                                QDialogButtonBox::Discard   |
                                                QDialogButtonBox::Help      |
                                                QDialogButtonBox::Apply     |
                                                QDialogButtonBox::Reset,
                                               &dlg };
        auto* btn       = buttons->button (button);

        QCOMPARE_NE (btn, NULL);

        QSignalSpy spy{ btn, &QAbstractButton::released };

        testutils::clickStandardButton (dlg, button);

        QCOMPARE (spy.count (), 1);
        }
    };

QTEST_MAIN (UiTest)

#include "uitest.moc"
