/**
 * @file        virtual/vestoptest.cpp
 * @brief       Test suite for the virtual estop controller
 * @author      Justin Scott
 * @date        2026-10-02
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#define LAYOUT_TEST_CLASS     VirtualEmergencyStopTest

#include <testutils/controllerstubs.hpp>
#include <testutils/ext/fakeit.hpp>

#include <layout/virtual/vestop.hpp>

#include <QtTest>

static const size_t ID = 666;

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the virtual estop controller
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class VirtualEmergencyStopTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for virtualStopTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void virtualStopTest_data ()
        {
        QTest::addColumn<int> ("initialSpeed");

        for (int ii = INT8_MIN; ii < INT8_MAX; ++ii)
            {
            QTest::addRow ("%d", ii) << ii;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the estop command for a virtual emergency stop controller
    ///
    /// @see    layout::VirtualEmergencyStopController::eStop()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void virtualStopTest ()
        {
        QFETCH (int, initialSpeed);

        testutils::LocomotiveControllerStubs        fakeLoco;
        fakeit::Mock<layout::LocomotiveController>  mockLoco{ fakeLoco };
        layout::VirtualEmergencyStopController      estop{ mockLoco.get () };

        layout::Locomotive loco{ &mockLoco.get (),
                                  "",
                                  layout::TRACK_PROTO_DCC128,
                                  1,
                                  {},
                                  static_cast<int8_t> (initialSpeed),
                                  ID };

        fakeit::When (Method (mockLoco, setSpeed)).AlwaysReturn ();
        fakeit::When (Method (mockLoco, getLocomotives)).AlwaysReturn ({ loco });

        estop.eStop (true);

        QVERIFY (estop.isEStopped ());

        // Verify that speed has been set to 0
        fakeit::Verify (Method (mockLoco, setSpeed).Using (ID, 0));

        estop.eStop (false);

        QVERIFY (not estop.isEStopped ());

        // Verify that speed has been restored to it's intial value
        fakeit::Verify (Method (mockLoco, setSpeed).Using (ID, initialSpeed));
        }
    };

QTEST_GUILESS_MAIN (VirtualEmergencyStopTest)

#include "vestoptest.moc"
