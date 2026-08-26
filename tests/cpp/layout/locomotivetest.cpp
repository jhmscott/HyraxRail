/**
 * @file        layout/locomotivetest.hpp
 * @brief       Test suite for the locomotive layout component
 * @author      Justin Scott
 * @date        2026-08-21
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#define LAYOUT_TEST_CLASS LocomotiveTest

#include <layout/locomotive.hpp>

#include <testutils/ext/fakeit.hpp>

#include <QtTest>

static constexpr size_t ID = 7890;

static const std::string            INIT_NAME = "Original Name";
static const layout::trackProtocol  INIT_PROTO= layout::TRACK_PROTO_MFX;
static const uint                   INIT_ADDR = 314159;


static const std::string            NEW_NAME  = "New Name";
static const layout::trackProtocol  NEW_PROTO = layout::TRACK_PROTO_MM14;
static const uint                   NEW_ADDR  = 271828;

///////////////////////////////////////////////////////////////////////////////
/// Assert the locomotive contains the initial state
///
/// @param[in]  loco        Locomotive to test
///
///////////////////////////////////////////////////////////////////////////////
static void assertInitState (const layout::Locomotive& loco)
    {
    QCOMPARE (loco.getId (),        ID);
    QCOMPARE (loco.getName (),      INIT_NAME);
    QCOMPARE (loco.getProtocol (),  INIT_PROTO);
    QCOMPARE (loco.getAddress (),   INIT_ADDR);
    }

static void assertNewState (const layout::Locomotive& loco)
    {
    QCOMPARE (loco.getId (),        ID);
    QCOMPARE (loco.getName (),      NEW_NAME);
    QCOMPARE (loco.getProtocol (),  NEW_PROTO);
    QCOMPARE (loco.getAddress (),   NEW_ADDR);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the locomotive layout component
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class LocomotiveTest : public QObject
    {
    Q_OBJECT
private:
    using FakeController = layout::ControllerBase<layout::Locomotive>;
    using MockController = fakeit::Mock<layout::LocomotiveController>;

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
            new MockController{ static_cast<layout::LocomotiveController&> (fakeController) });

        // Setup mock functions needed for all tests
        fakeit::When (Method (*mockController, requestControl)).AlwaysReturn ();
        fakeit::When (Method (*mockController, releaseControl)).AlwaysReturn ();
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
    /// Test the locomotive copy and assignment operators
    ///
    /// @see        layout::Locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void copyTest ()
        {
        layout::Locomotive loco{ &mockController->get (),
                                  INIT_NAME,
                                  INIT_PROTO,
                                  INIT_ADDR,
                                  ID };
        layout::Locomotive locoCopy = loco;

        assertInitState (locoCopy);

        layout::Locomotive locoAssign;

        locoAssign = loco;

        assertInitState (locoAssign);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for speedTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void speedTest_data ()
        {
        QTest::addColumn<int> ("speed");

        for (int ii = INT8_MIN; ii <= INT8_MAX; ++ii)
            {
            QTest::addRow ("Speed %d", ii) << ii;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test controlling the speed of the locomotive
    ///
    /// @see    layout::Locomotive::setSpeed();
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void speedTest ()
        {
        QFETCH (int, speed);

        fakeit::When (Method (*mockController, setSpeed)).AlwaysReturn ();

        layout::Locomotive loco{ &mockController->get (),
                                  INIT_NAME,
                                  INIT_PROTO,
                                  INIT_ADDR,
                                  ID };
        loco.setSpeed (speed);

        fakeit::Verify (Method (*mockController, setSpeed).Using (ID, speed));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the shared state of a locomotive between multiple instances of the same
    /// loco
    ///
    /// @see    layout::Locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void sharedStateTest ()
        {
        fakeit::When (Method (*mockController, setLocomotiveName)).AlwaysReturn ();
        fakeit::When (Method (*mockController, setLocomotiveAddress)).AlwaysReturn ();
        fakeit::When (Method (*mockController, setLocomotiveProtocol)).AlwaysReturn ();

        layout::Locomotive loco{ &mockController->get (),
                                  INIT_NAME,
                                  INIT_PROTO,
                                  INIT_ADDR,
                                  ID };
        layout::Locomotive locoCopy = loco;

        loco.setName    (NEW_NAME);
        loco.setAddress (NEW_ADDR);
        loco.setProtocol(NEW_PROTO);

        assertNewState (loco);
        assertNewState (locoCopy);

        fakeit::Verify (Method (*mockController, setLocomotiveName).    Using (ID, NEW_NAME));
        fakeit::Verify (Method (*mockController, setLocomotiveAddress). Using (ID, NEW_ADDR));
        fakeit::Verify (Method (*mockController, setLocomotiveProtocol).Using (ID, NEW_PROTO));
        }
    };

QTEST_GUILESS_MAIN (LocomotiveTest)

#include "locomotivetest.moc"
