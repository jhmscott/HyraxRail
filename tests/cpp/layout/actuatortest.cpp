/**
 * @file        layout/actuatortest.hpp
 * @brief       Test suite for the actuator layout component
 * @author      Justin Scott
 * @date        2026-08-21
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#define LAYOUT_TEST_CLASS ActuatorTest

#include <layout/actuator.hpp>

#include <testutils/ext/fakeit.hpp>

#include <QtTest>

static constexpr size_t ID = 123456;

static const std::string            INITIAL_NAME = "Initial Name";
static const layout::actuatorIcon   INITIAL_ICON = layout::ICON_STREET_LIGHT;
static const layout::actuatorMode   INITIAL_MODE = layout::actuatorMode::PULSE;
static const uint                   INITIAL_DUR  = 200;
static const uint                   INITIAL_ADDR = 21;


static const std::string            NEW_NAME    = "New Name";
static const layout::actuatorIcon   NEW_ICON    = layout::ICON_MAST_LIGHT;
static const layout::actuatorMode   NEW_MODE    = layout::actuatorMode::SWITCH;
static const uint                   NEW_DUR     = 100;
static const uint                   NEW_ADDR    = 42;


///////////////////////////////////////////////////////////////////////////////
/// Confirm that the actuator has been assigned the new state
///
/// @param[in]  actuator        Actuator to test
///
///////////////////////////////////////////////////////////////////////////////
static void assetNewActuatorState (const layout::Actuator& actuator)
    {
    QCOMPARE (actuator.getName (),      NEW_NAME);
    QCOMPARE (actuator.getIcon (),      NEW_ICON);
    QCOMPARE (actuator.getMode (),      NEW_MODE);
    QCOMPARE (actuator.getAddress (),   NEW_ADDR);
    QCOMPARE (actuator.getDuration (),  NEW_DUR);
    }


///////////////////////////////////////////////////////////////////////////////
/// Confirm that the actuator has been assigned the initial state
///
/// @param[in]  actuator        Actuator to test
///
///////////////////////////////////////////////////////////////////////////////
static void assetInitActuatorState (const layout::Actuator& actuator)
    {
    QCOMPARE (actuator.getId (),        ID);
    QCOMPARE (actuator.getName (),      INITIAL_NAME);
    QCOMPARE (actuator.getIcon (),      INITIAL_ICON);
    QCOMPARE (actuator.getMode (),      INITIAL_MODE);
    QCOMPARE (actuator.getDuration (),  INITIAL_DUR);
    QCOMPARE (actuator.getAddress (),   INITIAL_ADDR);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the actuator layout component
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class ActuatorTest : public QObject
    {
    Q_OBJECT
private:
    using FakeController = layout::ControllerBase<layout::Actuator>;
    using MockController = fakeit::Mock<layout::ActuatorController>;

    FakeController                  fakeController;
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
    /// Test the copy constructor and assignment operators
    ///
    /// @see    layout::Actuator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void copyTest ()
        {
        layout::Actuator actuator{ &mockController->get (),
                                    INITIAL_NAME,
                                    INITIAL_ICON,
                                    INITIAL_MODE,
                                    INITIAL_ADDR,
                                    INITIAL_DUR,
                                    ID,
                                    true };
        layout::Actuator actuatorCopy = actuator;


        // Assert the copy was correct
        assetInitActuatorState (actuatorCopy);

        layout::Actuator actuatorAssign;

        actuatorAssign = actuator;

        // Assert the assignment was correct
        assetInitActuatorState (actuatorAssign);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the shared state between multiple actuators and their parent controller
    ///
    /// @see    layout::Actuator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void sharedStateTest ()
        {
        fakeit::When (Method (*mockController, setActuatorMode)).AlwaysReturn ();
        fakeit::When (Method (*mockController, setActuatorName)).AlwaysReturn ();
        fakeit::When (Method (*mockController, setActuatorAddress)).AlwaysReturn ();
        fakeit::When (Method (*mockController, setActuatorDuration)).AlwaysReturn ();
        fakeit::When (Method (*mockController, setActuatorIcon)).AlwaysReturn ();

        layout::Actuator actuator{ &mockController->get (),
                                    INITIAL_NAME,
                                    INITIAL_ICON,
                                    INITIAL_MODE,
                                    INITIAL_ADDR,
                                    INITIAL_DUR,
                                    ID,
                                    true };
        layout::Actuator actuatorCopy = actuator;

        actuator.setName (NEW_NAME);
        actuator.setIcon (NEW_ICON);
        actuator.setMode (NEW_MODE);
        actuator.setAddress (NEW_ADDR);
        actuator.setDuration (NEW_DUR);

        // Since state is shared accross all actuators with the same ID,
        // it should have been updated for both the original and copy
        //
        assetNewActuatorState (actuator);
        assetNewActuatorState (actuatorCopy);

        // Now check that we've tried to write the new state back to the controller
        fakeit::Verify (Method (*mockController, setActuatorMode)       .Using (ID, NEW_MODE));
        fakeit::Verify (Method (*mockController, setActuatorName)       .Using (ID, NEW_NAME));
        fakeit::Verify (Method (*mockController, setActuatorAddress)    .Using (ID, NEW_ADDR));
        fakeit::Verify (Method (*mockController, setActuatorDuration)   .Using (ID, NEW_DUR));
        fakeit::Verify (Method (*mockController, setActuatorIcon)       .Using (ID, NEW_ICON));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for setActuatorTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setActuatorTest_data ()
        {
        QTest::addColumn<bool> ("initial");

        QTest::newRow ("true") << true;
        QTest::newRow ("false") << false;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the actuator set() member function of the layout::Actuator class
    ///
    /// @see    layout::Actuator::set()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setActuatorTest ()
        {
        QFETCH (bool, initial);

        fakeit::When (Method (*mockController, setActuator)).AlwaysReturn ();

        layout::Actuator actuator{ &mockController->get (),
                                    INITIAL_NAME,
                                    INITIAL_ICON,
                                    INITIAL_MODE,
                                    INITIAL_ADDR,
                                    INITIAL_DUR,
                                    ID,
                                    initial };
        layout::Actuator actuatorCopy = actuator;

        QSignalSpy spy{ &actuator, &layout::Actuator::stateChanged };
        QSignalSpy copySpy{ &actuatorCopy, &layout::Actuator::stateChanged };

        actuator.set (initial);

        // Set it to the existing state shouldn't trigger signals
        QCOMPARE (spy.size (), 0);
        QCOMPARE (copySpy.size (), 0);

        fakeit::Verify (Method (*mockController, setActuator)).Never ();

        bool newState = not initial;

        actuator.set (newState);

        // Changing the state should have signaled
        QCOMPARE (spy.size (), 1);
        QCOMPARE (copySpy.size (), 1);

        QCOMPARE (spy[0][0].toBool (), newState);
        QCOMPARE (copySpy[0][0].toBool (), newState);

        fakeit::Verify (Method (*mockController, setActuator).Using (ID, newState));
        }
    };

QTEST_GUILESS_MAIN (ActuatorTest)

#include "actuatortest.moc"
