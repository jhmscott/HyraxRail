/**
 * @file        testutils_test/uitest.cpp
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
/// Generate test data for all standard buttons
///
/// @tparam     Standard button parent (i.e. QMessageBox or QDialogButtonBox)
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
static void generateStandardButtons ()
    {
    QTest::addColumn<typename T::StandardButton> ("button");

    QTest::newRow ("Ok")            << T::Ok;
    QTest::newRow ("Save")          << T::Save;
    QTest::newRow ("Save All")      << T::SaveAll;
    QTest::newRow ("Open")          << T::Open;
    QTest::newRow ("Yes")           << T::Yes;
    QTest::newRow ("Yes to All")    << T::YesToAll;
    QTest::newRow ("No")            << T::No;
    QTest::newRow ("No to All")     << T::NoToAll;
    QTest::newRow ("Abort")         << T::Abort;
    QTest::newRow ("Retry")         << T::Retry;
    QTest::newRow ("Ignore")        << T::Ignore;
    QTest::newRow ("Close")         << T::Close;
    QTest::newRow ("Cancel")        << T::Cancel;
    QTest::newRow ("Discard")       << T::Discard;
    QTest::newRow ("Help")          << T::Help;
    QTest::newRow ("Apply")         << T::Apply;
    QTest::newRow ("Reset")         << T::Reset;
    }

///////////////////////////////////////////////////////////////////////////////
/// Create a set of flags for all standard buttons
///
/// @tparam     Standard button parent (i.e. QMessageBox or QDialogButtonBox)
///
/// @return     Standard button flags
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
static constexpr typename T::StandardButtons allStandardButtons ()
    {
    return  T::Ok        |
            T::Save      |
            T::SaveAll   |
            T::Open      |
            T::Yes       |
            T::YesToAll  |
            T::No        |
            T::NoToAll   |
            T::Abort     |
            T::Retry     |
            T::Ignore    |
            T::Cancel    |
            T::Discard   |
            T::Help      |
            T::Apply     |
            T::Reset;
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
    void clickStandardButtonTest_data () { generateStandardButtons<QDialogButtonBox> (); }

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

        auto* buttons   = new QDialogButtonBox{ allStandardButtons<QDialogButtonBox> (),
                                               &dlg };
        auto* btn       = buttons->button (button);

        QCOMPARE_NE (btn, NULL);

        QSignalSpy spy{ btn, &QAbstractButton::released };

        testutils::clickStandardButton (dlg, button);

        QCOMPARE (spy.count (), 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for clickMessageBoxButton()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clickMessageBoxButton_data () { generateStandardButtons<QMessageBox> (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests clicking a standard message box button
    ///
    /// @see    testutils::clickStandardButton()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clickMessageBoxButton ()
        {
        QFETCH (QMessageBox::StandardButton, button);

        QMessageBox mbox;

        mbox.setStandardButtons (allStandardButtons<QMessageBox> ());

        auto* btn = mbox.button (button);

        QSignalSpy spy{ btn, &QAbstractButton::released };

        testutils::clickStandardButton (mbox, button);

        QCOMPARE (spy.count (), 1);
        }
    };

QTEST_MAIN (UiTest)

#include "uitest.moc"
