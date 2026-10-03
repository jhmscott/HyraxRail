/**
 * @file        controlllers/basetest.cpp
 * @brief       Test suite for the Controller base class
 * @author      Justin Scott
 * @date        2026-09-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/controllers/base.hpp>

#include <testutils/controllerstubs.hpp>
#include <testutils/mockcontroller.hpp>

#include <QtTest>

class LocomotiveController :
    public control::ControllerBase,
    public testutils::LocomotiveControllerStubs
    {
    CONTROLLER_DEFINE (LocomotiveController, "Locomotive Controller", testutils::MockControllerProtocol);
public:
    using control::ControllerBase::ControllerBase;
    };

class ActuatorController :
    public control::ControllerBase,
    public testutils::ActuatorControllerStubs
    {
    CONTROLLER_DEFINE (ActuatorController, "Actuator Controller", testutils::MockControllerProtocol);
public:
    using control::ControllerBase::ControllerBase;
    };

class RouteController :
    public control::ControllerBase,
    public testutils::RouteControllerStubs
    {
    CONTROLLER_DEFINE (RouteController, "Route Controller", testutils::MockControllerProtocol);
public:
    using control::ControllerBase::ControllerBase;
    };

class EmergencyStopController :
    public control::ControllerBase,
    public testutils::EmergencyStopControllerStubs
    {
    CONTROLLER_DEFINE (EmergencyStopController, "Emergency Stop Controller", testutils::MockControllerProtocol);
public:
    using control::ControllerBase::ControllerBase;
    };

///////////////////////////////////////////////////////////////////////////////
/// Test a controller type with a single hardware capability
///
/// @tparam     Controller      Controller type to test
/// @tparam     Capability      Capability controller to test it against
///
/// @param[in]  capability      Capability we expect
///
///////////////////////////////////////////////////////////////////////////////
template<class Controller, class Capability>
static void testSingleCapability (control::controllerCapability capability)
    {
    // Sanity tests
    static_assert (std::is_base_of_v<control::ControllerBase, Controller>,
                   "Must be controller type");
    static_assert (std::is_base_of_v<Capability, Controller>,
                   "Must derive from the capability we are testing");

    Controller controller{ "Test Controller",
        std::make_unique<testutils::MockControllerProtocol>(utils::device::deviceInfo{}) };

    const control::ControllerMetaClassBase& meta = controller.getMetaClass ();

    // Test the various capability interfaces
    QVERIFY  (meta.hasCapability (capability));
    QVERIFY  (meta.getCapabilities ()[capability]);
    QCOMPARE (meta.softwareCapabilities[capability], false);

    // This is test single capability, we only have one hardware capability
    for (int ii = 0; ii < control::NUM_CAPABILITIES; ++ii)
        {
        QCOMPARE (meta.hardwareCapabilities[ii], ii == capability);
        }

    // Test that this is available in hardware
    QCOMPARE (controller.getCapabilityController<Capability> (),
              static_cast<Capability*> (&controller));
    }


///////////////////////////////////////////////////////////////////////////////
/// Test a controller type with a single hardware capability implements an expected
/// software capability
///
/// @tparam     Controller      Controller type to test
/// @tparam     Capability      Capability controller to test it against
/// @tparam     SwCapability    Virtual controller class for that capability
///
/// @param[in]  capability      Capability we expect
///
///////////////////////////////////////////////////////////////////////////////
template<class Controller, class Capability, class SwCapability>
void testSwCapability (control::controllerCapability capability)
    {
    // Sanity tests
    static_assert (std::is_base_of_v<control::ControllerBase, Controller>,
                   "Must be controller type");
    static_assert (not std::is_base_of_v<Capability, Controller>,
                   "Must not derive from the capability we are testing");
    static_assert (std::is_base_of_v<Capability, SwCapability>,
                   "SW Capability must impliment capability");
    static_assert (std::is_base_of_v<layout::VirtualControllerBase, SwCapability>,
                   "Must be a virtual controller type");

    Controller controller{ "Test Controller",
        std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

    QVERIFY (controller.getMetaClass ().softwareCapabilities[capability]);

    auto* swController = controller.getCapabilityController<Capability> ();

    // Check that there's a capability controller, and it's virtual (not hardware defined)
    QCOMPARE_NE (swController, NULL);
    QCOMPARE_NE (dynamic_cast<SwCapability*> (swController), NULL);
    }


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the Controller base class
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class ControllerBaseTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test a controller with the hardware actuator capability
    ///
    /// @see    control::ControllerBase
    /// @see    control::CAPABILITY_ACTUATOR
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void actuatorCapabilityTest ()
        {
        testSingleCapability<ActuatorController,
                             layout::ActuatorController> (control::CAPABILITY_ACTUATOR);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test a controller with the hardware locomotive capability
    ///
    /// @see    control::ControllerBase
    /// @see    control::CAPABILITY_LOCOMOTIVE
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void locomotiveCapabilityTest ()
        {
        testSingleCapability<LocomotiveController,
                             layout::LocomotiveController> (control::CAPABILITY_LOCOMOTIVE);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test a controller with the hardware route capability
    ///
    /// @see    control::ControllerBase
    /// @see    control::CAPABILITY_ROUTE
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void routeCapabilityTest ()
        {
        testSingleCapability<RouteController,
                             layout::RouteController> (control::CAPABILITY_ROUTE);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test a controller with the hardware emergency stop capability
    ///
    /// @see    control::ControllerBase
    /// @see    control::CAPABILITY_ESTOP
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void estopCapabilityTest ()
        {
        testSingleCapability<EmergencyStopController,
                             layout::EmergencyStopController> (control::CAPABILITY_ESTOP);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test that a locomotive controller without hardware estop support implements
    /// a estop in software
    ///
    /// @see    control::ControllerBase
    /// @see    control::CAPABILITY_LOCOMOTIVE
    /// @see    control::CAPABILITY_ESTOP
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void virtualEstopControllerTest ()
        {
        testSwCapability<LocomotiveController,
                         layout::EmergencyStopController,
                         layout::VirtualEmergencyStopController> (control::CAPABILITY_ESTOP);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test that an actuator controller without hardware route support implements
    /// a route database in software
    ///
    /// @see    control::ControllerBase
    /// @see    control::CAPABILITY_ACTUATOR
    /// @see    control::CAPABILITY_ROUTE
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void virtualRouteControllerTest ()
        {
        testSwCapability<ActuatorController,
                         layout::RouteController,
                         layout::VirtualRouteController> (control::CAPABILITY_ROUTE);
        }
    };

QTEST_GUILESS_MAIN (ControllerBaseTest)

#include "basetest.moc"
