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

#include <testutils/ext/fakeit.hpp>

#include <QtTest>

using namespace ui::actuators;


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the Actuator button widget
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class ActuatorButtonTest : public QObject
    {
    Q_OBJECT
private slots:
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

        layout::ControllerBase<layout::Actuator> fakeController; // needed for storage of controller data
        fakeit::Mock<layout::ActuatorController> mockController{
            static_cast<layout::ActuatorController&> (fakeController) };

        // Setup mock functions
        fakeit::When (Method (mockController, requestActuatorControl)).AlwaysReturn ();
        fakeit::When (Method (mockController, releaseActuatorControl)).AlwaysReturn ();
        fakeit::When (Method (mockController, setActuator)).AlwaysReturn ();

        layout::Actuator actuator{ &mockController.get (),
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

        // Verify that the button has requested control from the controller
        fakeit::Verify (Method (mockController, requestActuatorControl).Using (FAKE_ID));
        QCOMPARE (btn.getState (), expected);

        for (int ii = 0; ii < NUM_PRESSES; ++ii)
            {
            QTest::mouseClick (&btn, Qt::LeftButton);

            // A click should toggle the actuator state
            expected = not expected;

            fakeit::Verify (Method (mockController, setActuator).Using (FAKE_ID, expected));
            QCOMPARE (btn.getState (), expected);
            }
        }

        // Verify actuator control has been released
        fakeit::Verify (Method (mockController, releaseActuatorControl).Using (FAKE_ID));
        }
    };

QTEST_MAIN (ActuatorButtonTest)

#include "actuatorbuttontest.moc"
