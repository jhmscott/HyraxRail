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

using namespace std::string_literals;

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


using namespace utils::device;

static const std::pair<std::string, deviceInfo> CONTROLLERS[] =
    {
        { "Test 1", { TYPE_SERIAL, serialInfo{ QSerialPortInfo{ "COM1" }, 9600 } } },
        { "Test 2", { TYPE_SERIAL, serialInfo{ QSerialPortInfo{ "COM2" }, 9600 } } },
        { "Test 3", { TYPE_TCP, socketInfo{ HostInfo::fromString ("google.ca", HostInfo::type::HOSTNAME), 443} } },
        { "Test 4", { TYPE_TCP, socketInfo{ HostInfo::fromString ("8.8.8.8",   HostInfo::type::IP), 80 } } },
        { "Test 5", { TYPE_TCP, socketInfo{ HostInfo::fromString ("google.ca", HostInfo::type::HOSTNAME), 443} } },
        { "Test 6", { TYPE_TCP, socketInfo{ HostInfo::fromString ("8.8.8.8",   HostInfo::type::IP), 80 } } },
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
    QCOMPARE (controller.template getCapabilityController<Capability> (),
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

    auto* swController = controller.template getCapabilityController<Capability> ();

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
    /// Test data for getterTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void getterTest_data ()
        {
        QTest::addColumn<std::string> ("name");
        QTest::addColumn<deviceInfo> ("device");

        for (const auto& [name, device] : CONTROLLERS)
            {
            QTest::newRow (name.c_str ()) << name << device;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Basic test of getters matching constructor
    ///
    /// @see    control::ControllerBase::getFriendlyName()
    /// @see    control::ControllerBase::getDeviceInfo()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void getterTest ()
        {
        QFETCH (std::string, name);
        QFETCH (deviceInfo,  device);

        testutils::MockController controller{ name,
            std::make_unique<testutils::MockControllerProtocol> (device) };

        QCOMPARE (controller.getFriendlyName (), name);
        QCOMPARE (controller.getDeviceInfo (), device);
        }

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

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the cache refresh function
    ///
    /// @see    control::ControllerBase::refresh()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void refreshTest ()
        {
        testutils::MockController controller { "Test Controller",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        controller.refresh ();

        fakeit::Verify (Method (*controller.actuatorController, getActuators)).Once ();
        fakeit::Verify (Method (*controller.locomotiveController, getLocomotives)).Once ();
        fakeit::Verify (Method (*controller.routeController, getRoutes)).Once ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for setFriendlyNameTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setFriendlyNameTest_data ()
        {
        QTest::addColumn<std::string> ("name");

        for (const auto& [name, _] : CONTROLLERS)
            {
            QTest::newRow (name.c_str ()) << name;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting the controller's friendly name
    ///
    /// @see    control::ControllerBase::setFriendlyName()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setFriendlyNameTest ()
        {
        QFETCH (std::string, name);

        testutils::MockController controller{ "Test Controller",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        controller.setFriendlyName (name);

        QCOMPARE (controller.getFriendlyName (), name);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for setDeviceInfoTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setDeviceInfoTest_data ()
        {
        QTest::addColumn<deviceInfo> ("device");

        for (const auto& [_, device] : CONTROLLERS)
            {
            std::string name;

            if (TYPE_SERIAL == device.type)
                {
                auto info = std::get<serialInfo> (device.info);

                name = info.port.portName ().toStdString () + "@" +
                       std::to_string (info.baud);
                }
            else
                {
                auto        info = std::get<socketInfo> (device.info);
                const char* type = TYPE_TCP == device.type ? "TCP://" : "UDP://";

                name = type + info.host.toString ().toStdString () + ":" +
                       std::to_string (info.port);
                }

            QTest::newRow (name.c_str ()) << device;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting the device info
    ///
    /// @see    control::ControllerBase::setDeviceInfo()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setDeviceInfoTest ()
        {
        QFETCH (deviceInfo, device);

        testutils::MockController controller{ "Test Controller",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        controller.setDeviceInfo (device);
        controller.waitForMessages ();

        QCOMPARE (controller.getDeviceInfo (), device);
        }
    };

QTEST_GUILESS_MAIN (ControllerBaseTest)

#include "basetest.moc"
