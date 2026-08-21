/**
 * @file        common/actuatorbuttontest.hpp
 * @brief       Test suite for the actuator button
 * @author      Justin Scott
 * @date        2026-08-16
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#define LAYOUT_TEST_CLASS ActuatorButtonTest

#include <layout/actuator.hpp>

#include <ui/actuators/actuatorbutton.hpp>
#include <ui/actuators/editactuator.hpp>
#include <ui/common/utils.hpp>

#include <testutils/ext/fakeit.hpp>
#include <testutils/ui.hpp>

#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QtTest>

using namespace ui::actuators;

using FakeController = layout::ControllerBase<layout::Actuator>;
using MockController = fakeit::Mock<layout::ActuatorController>;


///////////////////////////////////////////////////////////////////////////////
/// Validate the ActuatorButton context menu
///
/// @param[in]  menu        Menu to validate
///
///////////////////////////////////////////////////////////////////////////////
static void validateActuatorMenu (const QMenu& menu)
    {
    QList<QAction*> actions = menu.actions ();

    QCOMPARE (actions.count (), 2);
    QCOMPARE (actions.at (0)->text (), "Delete");
    QCOMPARE (actions.at (1)->text (), "Edit");
    }


///////////////////////////////////////////////////////////////////////////////
/// Test the delete actuator context menu item
///
/// @param[in]  btn     Button to press delete on
/// @param[in]  cancel  True to cancel the delete option, false to accept
/// @param[in]  name    Expected name of the actuator
///
///////////////////////////////////////////////////////////////////////////////
static void deleteActuator (const ActuatorButton& btn, bool cancel, const QString& name)
    {
    testutils::testMenuItem (btn,
                             0,
                             validateActuatorMenu,
        [&] () -> void
        {
        QMessageBox* mbox = btn.findChild<QMessageBox*> ();

        QCOMPARE_NE (mbox, NULL);
        QVERIFY (mbox->text ().contains (name));

        QTest::mouseClick (mbox->button (cancel ? QMessageBox::No : QMessageBox::Yes),
                            Qt::LeftButton);
        });
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the Actuator button widget
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class ActuatorButtonTest : public QObject
    {
    Q_OBJECT
private:
    FakeController                  fakeController; // needed for storage of controller data
    std::unique_ptr<MockController> mockController;

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Init the test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void init ()
        {
        mockController.reset (
            new MockController{ static_cast<layout::ActuatorController&> (fakeController) });

        // Setup mock functions needed for all tests
        fakeit::When (Method (*mockController, requestActuatorControl)).AlwaysReturn ();
        fakeit::When (Method (*mockController, releaseActuatorControl)).AlwaysReturn ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Cleanup after the test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cleanup ()
        {
        mockController.reset ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for pressButtonTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void pressButtonTest_data ()
        {
        QTest::addColumn<bool> ("initial");

        QTest::newRow ("true")  << true;
        QTest::newRow ("false") << false;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test automated pressing of the actuator button
    ///
    /// @see    ui::actuators::ActuatorButton
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void pressButtonTest ()
        {
        static constexpr size_t FAKE_ID         = 314159;
        static constexpr int    NUM_PRESSES     = 16;

        QFETCH (bool, initial);

        fakeit::When (Method (*mockController, setActuator)).AlwaysReturn ();

        layout::Actuator actuator{ &mockController->get (),
                                    "Test Actuator",
                                    layout::ICON_MAST_LIGHT,
                                    layout::actuatorMode::PULSE,
                                    1,
                                    200,
                                    FAKE_ID,
                                    initial };

        {
        bool            expected = initial;
        ActuatorButton  btn{ actuator, NULL };

        btn.show ();

        // Verify that the button has requested control from the controller
        fakeit::Verify (Method (*mockController, requestActuatorControl).Using (FAKE_ID));
        QCOMPARE (btn.getState (), expected);

        for (int ii = 0; ii < NUM_PRESSES; ++ii)
            {
            QTest::mouseClick (btn.windowHandle (), Qt::LeftButton);

            // A click should toggle the actuator state
            expected = not expected;

            fakeit::Verify (Method (*mockController, setActuator).Using (FAKE_ID, expected));
            QCOMPARE (btn.getState (), expected);
            }
        }

        // Verify actuator control has been released
        fakeit::Verify (Method (*mockController, releaseActuatorControl).Using (FAKE_ID));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test opening the context menu of actuator button and deleting the actuator
    ///
    /// @see    ui::actuators::ActuatorButton::actuatorDeleted()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void contextMenuDeleteTest ()
        {
        static const char* const    ACTUATOR_NAME   = "Test Actuator Name";
        static constexpr size_t     FAKE_ID         = 271828;

        fakeit::When (Method (*mockController, removeActuator)).AlwaysReturn ();

        layout::Actuator actuator{ &mockController->get (),
                                    ACTUATOR_NAME,
                                    layout::ICON_MAST_LIGHT,
                                    layout::actuatorMode::PULSE,
                                    1,
                                    200,
                                    FAKE_ID,
                                    false };

        QSignalSpy      spy{ &actuator, &layout::Actuator::destroyed };

        ActuatorButton  btn{ actuator, NULL };
        QSignalSpy      btnSpy{ &btn, &ActuatorButton::actuatorDeleted };

        btn.show ();

        // First test, press delete and then cancel
        deleteActuator (btn, true, ACTUATOR_NAME);

        // Confirm we haven't signalled destruction of the actuator
        QCOMPARE (spy.count (), 0);
        QCOMPARE (btnSpy.count (), 0);

        // Repeat this, but now accept the delete
        deleteActuator (btn, false, ACTUATOR_NAME);

        // This time we should have a destroy signal
        QCOMPARE (spy.count (), 1);
        QCOMPARE (btnSpy.count (), 1);
        fakeit::Verify (Method (*mockController, removeActuator).Using (FAKE_ID));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test opening the context menu of actuator button and opening the edit dialog.
    /// Note that the actual EditActuator dialog is tested in the EditActuatorTest suite
    ///
    /// @see    ui::actuators::ActuatorButton
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void contextMenuEditTest ()
        {
        static const char* const    ACTUATOR_NAME   = "Editable Actuator";
        static constexpr size_t     FAKE_ID         = 16180339887;

        layout::Actuator actuator{ &mockController->get (),
                                    ACTUATOR_NAME,
                                    layout::ICON_MAST_LIGHT,
                                    layout::actuatorMode::PULSE,
                                    1,
                                    200,
                                    FAKE_ID,
                                    false };

        ActuatorButton  btn{ actuator, NULL };

        btn.show ();

        testutils::testMenuItem (btn,
                                 1,
                                 validateActuatorMenu,
            [&] () -> void
            {
            EditActuatorDialog* editDialog = btn.findChild<EditActuatorDialog*> ();

            QCOMPARE_NE (editDialog, NULL);

            editDialog->close ();
            });
        }
    };

QTEST_MAIN (ActuatorButtonTest)

#include "actuatorbuttontest.moc"
