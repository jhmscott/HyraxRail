/**
 * @file        testutils/controllerstubs.hpp
 * @brief       Controller class stubs
 * @author      Justin Scott
 * @date        2026-09-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>

namespace testutils
{

/// All stubs needed for a locomotive controller
class LocomotiveControllerStubs : public layout::LocomotiveController
    {
public:
    virtual const layout::LocomotiveControllerMetaClass& getLocoMetaClass () const override
        {
        static layout::LocomotiveControllerMetaClass meta; return meta;
        }

    virtual uint getNumberOfFunctions (layout::trackProtocol proto) const override { return 0; }

    virtual std::vector<layout::Locomotive> getLocomotives () const override { return {}; }

    virtual layout::Locomotive createLocomotive (const std::string&                     name,
                                                 layout::trackProtocol                  proto,
                                                 uint                                   address,
                                                 const std::vector<layout::funcInfo>&   functions) override
        {
        return {};
        }

private:
    virtual void setSpeed (size_t id, int8_t speed) override {}

    virtual void setFunc (size_t id, uint8_t func, bool enable) override {}

    virtual void setLocomotiveFunctions (size_t id, const std::vector<layout::funcInfo>& functions) override {}

    virtual void setLocomotiveName (size_t id, const std::string& name) override {}

    virtual void setLocomotiveAddress (size_t id, uint address) override {}

    virtual void setLocomotiveProtocol (size_t id, layout::trackProtocol proto) override {}

    virtual void removeLocomotive (size_t id) override {}

    virtual void requestControl (size_t id) override {}

    virtual void releaseControl (size_t id) override {}
    };

/// All stubs needed for an actuator controller
class ActuatorControllerStubs : public layout::ActuatorController
    {
public:
    virtual std::vector<layout::Actuator> getActuators () const override { return {}; }

    virtual layout::Actuator createActuator (const std::string& name,
                                                uint                   address,
                                                layout::actuatorIcon   icon,
                                                layout::actuatorMode   mode,
                                                uint                   duration) override
        {
        return {};
        }

private:
    virtual void setActuator (size_t id, bool val) override {}

    virtual void setActuatorMode (size_t id, layout::actuatorMode mode) override {}

    virtual void setActuatorName (size_t id, const std::string& name) override {}

    virtual void setActuatorAddress (size_t id, uint address) override {}

    virtual void setActuatorDuration (size_t id, uint duration) override {}

    virtual void setActuatorIcon (size_t id, layout::actuatorIcon icon) override {}

    virtual void requestActuatorControl (size_t id) override {}

    virtual void releaseActuatorControl (size_t id) override {}

    virtual void removeActuator (size_t id) override {}
    };

/// All stubs needed for a route controller
class RouteControllerStubs : public layout::RouteController
    {
public:
    virtual layout::Route createRoute (const std::string& name,
                                        const layout::routeList& actuators) override
        {
        return {};
        }

    virtual std::vector<layout::Route> getRoutes () const override { return {}; }

private:
    virtual void setRoute (size_t id) override {}

    virtual void removeRoute (size_t id) override {}

    virtual void setRouteMembers (size_t id, const layout::routeList& members) override {}

    virtual void setRouteName (size_t id, const std::string& name) override {}

    virtual void requestRouteControl (size_t id) override {}

    virtual void releaseRouteControl (size_t id) override {}

    };

/// All stubs needed for an emergency stop controller
class EmergencyStopControllerStubs : public layout::EmergencyStopController
    {
public:
    virtual void eStop (bool stop) override {}

    virtual bool isEStopped () override { return false; }
    };

/// All stubs needed for a controller meta class
class ControllerMetaClassBaseStubs : public control::ControllerMetaClassBase
    {
public:
    using control::ControllerMetaClassBase::ControllerMetaClassBase;

    virtual std::unique_ptr<control::ControllerBase>
        create (const std::string&                  friendlyName,
                const std::string&                  protocol,
                const utils::device::deviceInfo&    info) const override { return NULL; }

private:
    virtual layout::ActuatorController* asActuatorController (control::ControllerBase* controller) const override { return NULL; }

    virtual layout::LocomotiveController* asLocomotiveController (control::ControllerBase* controller)  const override { return NULL; }

    virtual layout::RouteController* asRouteController (control::ControllerBase* controller) const override { return NULL; }

    virtual layout::EmergencyStopController* asEstopController (control::ControllerBase* controller) const override { return NULL; }
    };

class ControllerBaseStubs : public control::ControllerBase
    {
public:
    using control::ControllerBase::ControllerBase;

    virtual const control::ControllerMetaClassBase& getMetaClass () const override
        {
        static ControllerMetaClassBaseStubs meta{
        "ControllerBaseStubs",
        "Controller Base Stubs",
        { },
        control::controllerCapabilitySet{ 0 } };
        return meta;
        }
    };
}