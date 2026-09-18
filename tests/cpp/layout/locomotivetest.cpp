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

#include <testutils/assert.hpp>
#include <testutils/ext/fakeit.hpp>

#include <QtTest>

static constexpr size_t ID = 7890;

static const std::string                    INIT_NAME = "Original Name";
static const layout::trackProtocol          INIT_PROTO= layout::TRACK_PROTO_MFX;
static const uint                           INIT_ADDR = 314159;
static const std::vector<layout::funcInfo>  INIT_FUNCS=
    {
        { "Function 1", layout::funcInfo::ICON_FUNC_SOUND_GENERIC,  1, true  },
        { "Function 3", layout::funcInfo::ICON_FUNC_LIGHT_CAB,      3, false },
        { "Function 5", layout::funcInfo::ICON_FUNC_MISC_SLOW,      5, true  }
    };


static const std::string                    NEW_NAME  = "New Name";
static const layout::trackProtocol          NEW_PROTO = layout::TRACK_PROTO_MM14;
static const uint                           NEW_ADDR  = 271828;
static const std::vector<layout::funcInfo>  NEW_FUNCS =
    {
        { "Function 2", layout::funcInfo::ICON_FUNC_LIGHT_HEADLIGHT,2, false  },
        { "Function 4", layout::funcInfo::ICON_FUNC_SOUND_OPERATING,4, true   },
        { "Function 6", layout::funcInfo::ICON_FUNC_SOUND_HORN,     6, false  }
    };

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
    using FakeController = layout::ControllerBase<layout::Locomotive>;  ///< Controller type
    using MockController = fakeit::Mock<layout::LocomotiveController>;  ///< Controller mock type

    FakeController                  fakeController;                     ///< Controller used for testing
    std::unique_ptr<MockController> mockController;                     ///< Mocked controller interface

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
                                  INIT_FUNCS,
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
                                  INIT_FUNCS,
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
        fakeit::When (Method (*mockController, setLocomotiveFunctions)).AlwaysDo (
            [] (size_t id,const std::vector<layout::funcInfo>& funcs) -> void
            {
            QCOMPARE (id, ID);
            COMPARE_RANGE (funcs, NEW_FUNCS);
            });

        layout::Locomotive loco{ &mockController->get (),
                                  INIT_NAME,
                                  INIT_PROTO,
                                  INIT_ADDR,
                                  INIT_FUNCS,
                                  ID };
        layout::Locomotive locoCopy = loco;

        loco.setName        (NEW_NAME);
        loco.setAddress     (NEW_ADDR);
        loco.setProtocol    (NEW_PROTO);
        loco.setFunctions   (NEW_FUNCS);

        assertNewState (loco);
        assertNewState (locoCopy);

        fakeit::Verify (Method (*mockController, setLocomotiveName).    Using (ID, NEW_NAME));
        fakeit::Verify (Method (*mockController, setLocomotiveAddress). Using (ID, NEW_ADDR));
        fakeit::Verify (Method (*mockController, setLocomotiveProtocol).Using (ID, NEW_PROTO));
        fakeit::Verify (Method (*mockController, setLocomotiveFunctions));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the funcInfo comparison operators
    ///
    /// @see    layout::funcInfo::operator==()
    /// @see    layout::funcInfo::operator!=()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void funcInfoComparisonTest ()
        {
        for (int ii = 0; ii < NEW_FUNCS.size (); ++ii)
            {
            QCOMPARE (INIT_FUNCS[ii], INIT_FUNCS[ii]);
            QCOMPARE (NEW_FUNCS[ii], NEW_FUNCS[ii]);

            QCOMPARE_NE (NEW_FUNCS[ii], INIT_FUNCS[ii]);
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the funcInfo stream format operator
    ///
    /// @see    layout::funcInfo::operator<<()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void funcInfoStreamFormatTest ()
        {
        for (const auto& func : NEW_FUNCS)
            {
            std::stringstream   actual;
            std::string         expected;

            actual << func;

            expected = "{ " + func.name                     + ", " +
                              std::to_string (func.icon)    + ", " +
                              std::to_string (func.id)      + ", " +
                              (func.state ?  "on" : "off")  + " }";

            QCOMPARE (actual.str (), expected);
            }
        }
    };

QTEST_GUILESS_MAIN (LocomotiveTest)

#include "locomotivetest.moc"
