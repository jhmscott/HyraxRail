/**
 * @file        controlllers/metatest.cpp
 * @brief       Test suite for the Controller meta class
 * @author      Justin Scott
 * @date        2026-10-02
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/controllers/meta.hpp>

#include <testutils/controllerstubs.hpp>

#include <QtTest>

///////////////////////////////////////////////////////////////////////////////
/// Test the ability to detect a software capability from a hardware capability
///
/// @param[in]  hardware        Hardware capability to test
/// @param[in]  software        Software feature that should be available with
///                             that hardware feature
///
///////////////////////////////////////////////////////////////////////////////
static void testSoftwareCapabilities (control::controllerCapability hardware,
                                      control::controllerCapability software)
    {
    control::controllerCapabilitySet set{ 0 };

    // first try with the hardware capability alone, should have the software capability

    set[hardware] = true;

    testutils::ControllerMetaClassBaseStubs hasSoftware{ "Test1", "Test 1", {}, set };

    QCOMPARE (hasSoftware.hardwareCapabilities, set);       // sanity test
    QVERIFY (hasSoftware.softwareCapabilities[software]);   // actual test

    // Now test that if we have the capability in hardware, we don't implement it in software

    set[software] = true;

    testutils::ControllerMetaClassBaseStubs hasHardware{ "Test2", "Test 2", {}, set };

    QCOMPARE (hasHardware.hardwareCapabilities, set);           // sanity test
    QVERIFY (not hasHardware.softwareCapabilities[software]);   // actual test
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the Controller meta class
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class ControllerMetaTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test that a hardware locomotive controller results in a software estop
    /// controller
    ///
    /// @see    control::ControllerMetaClassBase
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void locomotiveHardwareEstopSoftwareTest ()
        {
        testSoftwareCapabilities (control::CAPABILITY_LOCOMOTIVE,
                                  control::CAPABILITY_ESTOP);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test that a hardware actuator controller results in a software route
    /// controller
    ///
    /// @see    control::ControllerMetaClassBase
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void actuatorHardwareRouteSoftwareTest ()
        {
        testSoftwareCapabilities (control::CAPABILITY_ACTUATOR,
                                  control::CAPABILITY_ROUTE);
        }
    };

QTEST_GUILESS_MAIN (ControllerMetaTest)

#include "metatest.moc"
