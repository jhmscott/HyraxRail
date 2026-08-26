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

#include <testutils/ext/fakeit.hpp>

#include <utils/pp.hpp>

namespace testutils
{
template<class T>
using MockPtr = std::unique_ptr<fakeit::Mock<T>>;

///////////////////////////////////////////////////////////////////////////////
/// Mocked controller's protcol defintion. Not meant to be used directly,
/// just required for the MockController's meta class
///
///////////////////////////////////////////////////////////////////////////////
class MockControllerProtocol : public control::ProtocolBase
    {
    PROTOCOL_DEFINE (MockControllerProtocol, "Mocked Protocol",
                     utils::device::TYPE_UDP, // Todo: mocked sockets
                     control::ProtocolMetaClassBase::NO_DEFAULT_PORT,
                     utils::device::TYPE_UDP);
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  deviceInfo      Device info
    ///////////////////////////////////////////////////////////////////////////////
    MockControllerProtocol (const utils::device::deviceInfo& deviceInfo) :
        control::ProtocolBase (deviceInfo, 1000)
        {}

    };

///////////////////////////////////////////////////////////////////////////////
/// Mocked controller class
///
///////////////////////////////////////////////////////////////////////////////
class MockController : public control::ControllerBase
    {
    CONTROLLER_DEFINE (MockController, "Mocked Controller", MockControllerProtocol);
public:
    MockPtr<layout::ActuatorController>    actuatorController;
    MockPtr<layout::LocomotiveController>  locomotiveController;
    MockPtr<layout::RouteController>       routeController;

    ///////////////////////////////////////////////////////////////////////////////
    /// Mocked controller constructor
    ///
    /// @param[in]  friendlyName    Controller name
    /// @param[in]  proto           Protocol
    ///
    ///////////////////////////////////////////////////////////////////////////////
    MockController (const std::string&                          friendlyName,
                    std::unique_ptr<control::ProtocolBase>&&    proto) :
        actuatorController      (new fakeit::Mock<layout::ActuatorController>   { *this }),
        locomotiveController    (new fakeit::Mock<layout::LocomotiveController> { *this }),
        routeController         (new fakeit::Mock<layout::RouteController>      { *this }),
        control::ControllerBase (friendlyName, std::move (proto))
        {}

    // Base controller stubs

    virtual std::vector<layout::Locomotive> getLocomotives () const override { return {}; }

    virtual std::vector<layout::Actuator> getActuators () const override { return {}; }

    virtual std::vector<layout::Route> getRoutes () const override { return {}; }

    virtual layout::Route createRoute (const std::string&       name,
                                       const layout::routeList& actuators) override { return {}; }

    virtual layout::Actuator createActuator (const std::string&     name,
                                             uint                   address,
                                             layout::actuatorIcon   icon,
                                             layout::actuatorMode   mode,
                                             uint                   duration) override { return {}; }

    virtual void eStop (bool stop) override {}

    virtual bool isEStopped () override { return false; }

    // Locomotive controller stubs

    virtual const layout::LocomotiveControllerMetaClass& getLocoMetaClass () const override { return {}; }

private:

    virtual void setSpeed (size_t id, int8_t speed) override {}

    virtual void setFunc (size_t id, uint8_t func, bool enable) override {}

    virtual std::vector<layout::funcInfo> getFunctions (size_t id) const override { return {}; }

    virtual void setLocomotiveName (size_t id, const std::string& name) override {}

    virtual void setLocomotiveAddress (size_t id, uint address) override {}

    virtual void setLocomotiveProtocol (size_t id, layout::trackProtocol proto) {}

    virtual void requestControl (size_t id) {}

    virtual void releaseControl (size_t id) {}

    // Actuator controller stubs

    virtual void setActuator (size_t id, bool val) override {}

    virtual void setActuatorMode (size_t id, layout::actuatorMode mode) override {}

    virtual void setActuatorName (size_t id, const std::string& name) override {}

    virtual void setActuatorAddress (size_t id, uint address) override {}

    virtual void setActuatorDuration (size_t id, uint duration) override {}

    virtual void setActuatorIcon (size_t id, layout::actuatorIcon icon) override {}

    virtual void requestActuatorControl (size_t id) override {}

    virtual void releaseActuatorControl (size_t id) override {}

    virtual void removeActuator (size_t id) override {}

    // route controller stubs

    virtual void setRoute (size_t id) override {}

    virtual void removeRoute (size_t id) override {}

    virtual void setRouteMembers (size_t id, const layout::routeList& members) override {}

    virtual void setRouteName (size_t id, const std::string& name) override {}

    virtual void requestRouteControl (size_t id) override {}

    virtual void releaseRouteControl (size_t id) override {}

    };

///////////////////////////////////////////////////////////////////////////////
/// Return a controller info for creating a mocked controller
///
/// @param[in]  name        Controller name
///
/// @return     Controller creation info
///
///////////////////////////////////////////////////////////////////////////////
control::createControllerInfo mockedControllerInfo (const std::string& name)
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