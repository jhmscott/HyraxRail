/**
 * @file        testutils/mockcontroller.hpp
 * @brief       Mocked controller class
 * @author      Justin Scott
 * @date        2026-08-23
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>

#include <testutils/controllerstubs.hpp>
#include <testutils/ext/fakeit.hpp>

#include <utils/pp.hpp>

namespace testutils
{
/// Smart pointer to a mocked object
template<class T>
using MockPtr = std::unique_ptr<fakeit::Mock<T>>;

///////////////////////////////////////////////////////////////////////////////
/// Mocked controller's protocol defintion. Not meant to be used directly,
/// just required for the MockController's meta class
///
///////////////////////////////////////////////////////////////////////////////
class MockControllerProtocol : public control::ProtocolBase
    {
    PROTOCOL_DEFINE (MockControllerProtocol, "Mocked Protocol",
                     utils::device::TYPE_UDP,
                     control::ProtocolMetaClassBase::NO_DEFAULT_PORT,
                     utils::device::TYPE_UDP)

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  deviceInfo      Device info
    ///
    ///////////////////////////////////////////////////////////////////////////////
    MockControllerProtocol (const utils::device::deviceInfo& deviceInfo) :
        control::ProtocolBase (deviceInfo, 1000)
        {}

    };

///////////////////////////////////////////////////////////////////////////////
/// Mocked controller class
///
///////////////////////////////////////////////////////////////////////////////
class MockController :
    public control::ControllerBase,
    public LocomotiveControllerStubs,
    public ActuatorControllerStubs,
    public RouteControllerStubs,
    public EmergencyStopControllerStubs
    {
    CONTROLLER_DEFINE (MockController, "Mocked Controller", MockControllerProtocol)
public:
    MockPtr<layout::ActuatorController>     actuatorController;     ///< Mocked actuator controller interface
    MockPtr<layout::LocomotiveController>   locomotiveController;   ///< Mocked loco controller interface
    MockPtr<layout::RouteController>        routeController;        ///< Mocked route controller interface
    MockPtr<layout::EmergencyStopController>estopController;        ///< Mocked estop controller interface

    std::vector<layout::Actuator>           actuators;
    std::vector<layout::Locomotive>         locomotives;
    std::vector<layout::Route>              routes;

    ///////////////////////////////////////////////////////////////////////////////
    /// Mocked controller constructor
    ///
    /// @param[in]  friendlyName    Controller name
    /// @param[in]  proto           Protocol
    ///
    ///////////////////////////////////////////////////////////////////////////////
    MockController (const std::string&                          friendlyName,
                    std::unique_ptr<control::ProtocolBase>&&    proto) :
        actuatorController      (new fakeit::Mock<layout::ActuatorController>       { *this }),
        locomotiveController    (new fakeit::Mock<layout::LocomotiveController>     { *this }),
        routeController         (new fakeit::Mock<layout::RouteController>          { *this }),
        estopController         (new fakeit::Mock<layout::EmergencyStopController>  { *this }),
        control::ControllerBase (friendlyName, std::move (proto))
        {
        fakeit::When (Method (*actuatorController, getActuators)).AlwaysDo (
            [this] () { return actuators; });

        fakeit::When (Method (*locomotiveController, getLocomotives)).AlwaysDo (
            [this] () { return locomotives; });

        fakeit::When (Method (*routeController, getRoutes)).AlwaysDo (
            [this] () { return routes; });
        }
    };

///////////////////////////////////////////////////////////////////////////////
/// Return a controller info for creating a mocked controller
///
/// @param[in]  name        Controller name
///
/// @return     Controller creation info
///
///////////////////////////////////////////////////////////////////////////////
inline control::createControllerInfo mockedControllerInfo (const std::string& name)
    {
    return control::createControllerInfo
        {
        UTILPP_STRINGIFY (MockController),
        name,
        UTILPP_STRINGIFY (MockControllerProtocol),
        utils::device::deviceInfo{}
        };
    }
}