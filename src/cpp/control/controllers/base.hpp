/**
 * @file        controller/base.hpp
 * @brief       Abstract base class for all model train controllers,
 *              and metaclass utilities
 * @author      Justin Scott
 * @date        2026-01-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/item.hpp>
#include <control/controllers/worker.hpp>
#include <control/protocols/base.hpp>

#include <layout/actuator.hpp>
#include <layout/estop.hpp>
#include <layout/locomotive.hpp>
#include <layout/route.hpp>


///////////////////////////////////////////////////////////////////////////////
/// Add this macro the defintion of your controller class to associate with a
/// meta class, giving this modules the ability to construct it programmatically
///
/// @param[in]  type            The controller type. This becomes the unique
///                             key for the controller
/// @param[in]  friendlyName    The name you wish to use for this controller
///                             in the UI
/// @param[in]  ...             Variadic list of protocol types this supports
///
/// @ingroup    META_CLASS_MACRO
///
///////////////////////////////////////////////////////////////////////////////
#define CONTROLLER_DEFINE(type, friendlyName, ...) \
    public:\
        static const control::ControllerMetaClassBase& getMetaClassStatic () { return meta; } \
        virtual const control::ControllerMetaClassBase& getMetaClass () const override { return meta; } \
    private:\
        static inline const control::ControllerMetaClass<type> meta{ #type, (friendlyName), control::internal::makeProtocolList<__VA_ARGS__> () };

namespace control
{
/// Capability a controller can have
enum controllerCapability
    {
    CAPABILITY_LOCOMOTIVE,  ///< @see layout::LocomotiveController
    CAPABILITY_ACTUATOR,    ///< @see layout::ActuatorController
    CAPABILITY_ROUTE,       ///< @see layout::RouteController
    CAPABILITY_ESTOP,       ///< @see layout::EmergencyStopController

    NUM_CAPABILITIES        ///< Delimiter only
    };

/// Set of capabilities supported by a controller
using controllerCapabilitySet = std::bitset<NUM_CAPABILITIES>;


/// List of protocols supported  by a controller
using protocolMetaList = std::vector<const ProtocolMetaClassBase*>;

/// internal controller namespace
namespace internal
{


///////////////////////////////////////////////////////////////////////////////
/// Type trait to get the enumerated capability type for a given layout controller
///
/// @tparam     Controller      Layout controller type
///
///////////////////////////////////////////////////////////////////////////////
template<class Controller>
struct capabilityTrait
    {};

template<>
struct capabilityTrait<layout::ActuatorController>
    {
    static constexpr controllerCapability value = CAPABILITY_ACTUATOR;
    };

template<>
struct capabilityTrait<layout::LocomotiveController>
    {
    static constexpr controllerCapability value = CAPABILITY_LOCOMOTIVE;
    };

template<>
struct capabilityTrait<layout::RouteController>
    {
    static constexpr controllerCapability value = CAPABILITY_ROUTE;
    };

template<>
struct capabilityTrait<layout::EmergencyStopController>
    {
    static constexpr controllerCapability value = CAPABILITY_ESTOP;
    };

///////////////////////////////////////////////////////////////////////////////
/// Generates a list of the protocol meta-class objects from a varidac list of
/// protocol types
///
/// @tparam     Protos          Variadic list of protocol types
///
/// @return     List of the meta classes for Protos
///
///////////////////////////////////////////////////////////////////////////////
template<class... Protos>
protocolMetaList makeProtocolList ()
    {
    static_assert (sizeof...(Protos) > 0, "Must pass at least one protocol");

    return { (&Protos::getMetaClassStatic ())...};
    }


///////////////////////////////////////////////////////////////////////////////
/// Determine for a derived conroller type the hardware capabilities available
///
/// @tparam     Controller      Type derived from ControllerBase and some set
///                             of layout controllers
///
/// @return     Capability set
///
///////////////////////////////////////////////////////////////////////////////
template<class Controller>
constexpr controllerCapabilitySet deduceHardwareCapabilites ()
    {
    controllerCapabilitySet capabilities{ 0 };

    if constexpr (std::is_base_of_v<layout::LocomotiveController, Controller>)
        {
        capabilities[CAPABILITY_LOCOMOTIVE] = true;
        }

    if constexpr (std::is_base_of_v<layout::ActuatorController, Controller>)
        {
        capabilities[CAPABILITY_ACTUATOR] = true;
        }

    if constexpr (std::is_base_of_v<layout::RouteController, Controller>)
        {
        capabilities[CAPABILITY_ROUTE] = true;
        }

    if constexpr (std::is_base_of_v<layout::EmergencyStopController, Controller>)
        {
        capabilities[CAPABILITY_ESTOP] = true;
        }

    return capabilities;
    }

} // namespace internal

// Forward declare
class ControllerBase;
struct createControllerInfo;

///////////////////////////////////////////////////////////////////////////////
/// Controller meta class abstract base class. Meta class is templated, so
/// this provides a concrete type with the meta class interface
///
/// @ingroup    META_CLASS
///
///////////////////////////////////////////////////////////////////////////////
class ControllerMetaClassBase
    {
    friend std::unique_ptr<ControllerBase> createController (const createControllerInfo& info);
    friend const std::vector<const ControllerMetaClassBase*> getControllers ();
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor. Inits the controller type information
    ///
    /// @param[in]  name            Configuration name of the controller.
    ///                             Used in the registry and to programmatically
    ///                             construct. This must be unique and is typically
    ///                             created from the type name
    /// @param[in]  friendlyName    Name of the controller type to use in the UI
    /// @param[in]  protocols       List of protocols this controller supports
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ControllerMetaClassBase (const std::string&             name,
                             const std::string&             friendlyName,
                             const protocolMetaList&        protocols,
                             const controllerCapabilitySet& capabilities);

    const std::string               name;                   ///< Type name (used internally)
    const std::string               friendlyName;           ///< Friendly name (used in UI)
    const protocolMetaList          protocols;              ///< List of protocols
    const controllerCapabilitySet   hardwareCapabilities;   ///< Capabilities the controller natively supports
    const controllerCapabilitySet   softwareCapabilities;   ///< Capabilities the app can emulate
                                                            ///  for the controller in software

    ///////////////////////////////////////////////////////////////////////////////
    /// Create an instance of this controller type
    ///
    /// @param[in]  friendlyName    Name of this controller instance to use in the UI
    /// @param[in]  protocol        Name of the protocol type
    /// @param[in]  info            Used to create the device to communicate with
    ///                             the controller
    ///
    /// @return     unique_ptr instance of the requested controller
    ///             nullptr if the controller could not be created
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ControllerBase>
        create (const std::string&                  friendlyName,
                const std::string&                  protocol,
                const utils::device::deviceInfo&    info) const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Find the a protocol by name
    ///
    /// @param[in]  protocol        Protocol type name
    ///
    /// @return     Protocol Found
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const ProtocolMetaClassBase& findProtocol (const std::string& protocol) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the controller type's capabilities, both hardware and software
    ///
    /// @return     Capabilities
    ///
    ///////////////////////////////////////////////////////////////////////////////
    controllerCapabilitySet getCapabilities () const
        { return hardwareCapabilities | softwareCapabilities; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if this controller has a given capability
    ///
    /// @param[in]  capability      Capability to check
    ///
    /// @return     true if it has it
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool hasCapability (controllerCapability capability) const
        { return getCapabilities ()[capability]; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the meta class for a controller
    ///
    /// @param[in]  controller      Controller instance
    ///
    /// @return     Meta class for controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static const ControllerMetaClassBase& fromController (const ControllerBase& controller);

    ///////////////////////////////////////////////////////////////////////////////
    /// Cast a controller to one of the capability classes, if that controller has
    /// the given capability in hardware
    ///
    /// @tparam     Capability      Capability controller class
    ///
    /// @param[in]  controller      Base controller to cast as capability
    ///
    /// @return     Controller instance as capability class
    ///             NULL if controller lacks that capability
    ///
    /// @remarks    Why is this not just dynamic_cast? dynamic_cast does not work
    ///             on mocked classes, which is used extensively in the unit tests.
    ///             So my own equivalent was needed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class Capability>
    static Capability* cast (ControllerBase* controller)
        {
        static_assert (utils::traits::always_false_v<Capability>,
                       "Undefined Capability controller class");
        return NULL;
        }

    template<>
    static layout::ActuatorController* cast<layout::ActuatorController> (ControllerBase* controller)
        {
        return fromController (*controller).asActuatorController (controller);
        }

    template<>
    static layout::LocomotiveController* cast<layout::LocomotiveController> (ControllerBase* controller)
        {
        return fromController (*controller).asLocomotiveController (controller);
        }

    template<>
    static layout::RouteController* cast<layout::RouteController> (ControllerBase* controller)
        {
        return fromController (*controller).asRouteController (controller);
        }

    template<>
    static layout::EmergencyStopController* cast<layout::EmergencyStopController> (ControllerBase* controller)
        {
        return fromController (*controller).asEstopController (controller);
        }

    template<class Capability>
    static const Capability* cast (const ControllerBase* controller)
        {
        return cast<Capability*> (const_cast<ControllerBase*> (controller));
        }

private:
    /// Supported controller types populated automatically by metaclass global constructors
    static inline std::map<std::string, ControllerMetaClassBase*> controllerTypes;

    ///////////////////////////////////////////////////////////////////////////////
    /// Cast implementation of controller base -> actuator controller
    ///
    /// @param[in]  controller      Controller base
    ///
    /// @return     Actuator controller if controller has the actuator capability
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual layout::ActuatorController* asActuatorController (ControllerBase* controller) const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Cast implementation of controller base -> locomotive controller
    ///
    /// @param[in]  controller      Controller base
    ///
    /// @return     Locomotive controller if controller has the locomotive capability
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual layout::LocomotiveController* asLocomotiveController (ControllerBase* controller) const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Cast implementation of controller base -> route controller
    ///
    /// @param[in]  controller      Controller base
    ///
    /// @return     Route controller if controller has the route capability
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual layout::RouteController* asRouteController (ControllerBase* controller) const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Cast implementation of controller base -> estop controller
    ///
    /// @param[in]  controller      Controller base
    ///
    /// @return     estop controller if controller has the route capability
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual layout::EmergencyStopController* asEstopController (ControllerBase* controller) const = 0;
    };


///////////////////////////////////////////////////////////////////////////////
/// Meta class derived type. Implements logic specific to the controller type
///
/// @tparam     T           Controller type. Must derive from ControllerBase
///
/// @ingroup    META_CLASS
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
class ControllerMetaClass : public ControllerMetaClassBase
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor. Inits the controller type information
    ///
    /// @param[in]  name            Configuration name of the controller.
    ///                             Used in the registry and to programmatically
    ///                             construct. This must be unique and is typically
    ///                             created from the type name
    /// @param[in]  friendlyName    Name of the controller type to use in the UI
    /// @param[in]  protocols       List of protocols this controller supports
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ControllerMetaClass (const std::string&         name,
                         const std::string&         friendlyName,
                         const protocolMetaList&    protocols) :
        ControllerMetaClassBase (name, friendlyName, protocols, internal::deduceHardwareCapabilites<T> ())
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Create an instance of this controller type
    ///
    /// @param[in]  friendlyName    Name of this controller instance to use in the UI
    /// @param[in]  protocol        Name of the protocol type
    /// @param[in]  info            Used to create the device to communicate with the controller
    ///
    /// @return     unique_ptr instance of the requested controller
    ///             nullptr if the controller could not be created
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ControllerBase>
        create (const std::string&                  friendlyName,
                const std::string&                  protocol,
                const utils::device::deviceInfo&    info) const override
        {
        static_assert (std::is_base_of_v<ControllerBase, T>,
                       "Meta class must be used with a controller class");

        return std::make_unique<T> (friendlyName, findProtocol (protocol).create (info));
        }
private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Implementation of the cast to layout controller operator
    ///
    /// @tparam     Controller      Capability controller to case to
    ///
    /// @param[in]  controller      Controller instance
    ///
    /// @return     Controller as a Capability controller instance if it hase that capability
    ///             NULL if it does not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class Controller>
    Controller* castImpl (ControllerBase* controller) const
        {
        if constexpr (std::is_base_of_v<Controller, T>)
            {
            return static_cast<T*> (controller);
            }
        else
            {
            return NULL;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Cast implementation of controller base -> actuator controller
    ///
    /// @param[in]  controller      Controller base
    ///
    /// @return     Actuator controller if controller has the actuator capability
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual layout::ActuatorController* asActuatorController (ControllerBase* controller) const override
        {
        return castImpl<layout::ActuatorController> (controller);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Cast implementation of controller base -> locomotive controller
    ///
    /// @param[in]  controller      Controller base
    ///
    /// @return     Locomotive controller if controller has the locomotive capability
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual layout::LocomotiveController* asLocomotiveController (ControllerBase* controller) const override
        {
        return castImpl<layout::LocomotiveController> (controller);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Cast implementation of controller base -> route controller
    ///
    /// @param[in]  controller      Controller base
    ///
    /// @return     Route controller if controller has the route capability
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual layout::RouteController* asRouteController (ControllerBase* controller) const override
        {
        return castImpl<layout::RouteController> (controller);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Cast implementation of controller base -> estop controller
    ///
    /// @param[in]  controller      Controller base
    ///
    /// @return     estop controller if controller has the route capability
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual layout::EmergencyStopController* asEstopController (ControllerBase* controller) const override
        {
        return castImpl<layout::EmergencyStopController> (controller);
        }
    };


///////////////////////////////////////////////////////////////////////////////
/// Base class for all model train controller
///
/// @remarks    Derived classes must include CONTROLLER_DEFINE(...) within their defintion
///             to take advantage of the meta class features.
///
///////////////////////////////////////////////////////////////////////////////
class ControllerBase
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  friendlyName        Name of thi controller in UI
    /// @param[in]  proto               Protocol to use. Controller takes ownership
    ///
    /// @remarks    Derived classes must have the same function signature
    ///             for their constructors
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ControllerBase (const std::string&              friendlyName,
                    std::unique_ptr<ProtocolBase>&& proto);

    ///////////////////////////////////////////////////////////////////////////////
    /// Virtual destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~ControllerBase () {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the locomotives under the control of this controller
    ///
    /// @return     List of locomotives
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::vector<layout::Locomotive> getLocomotives () const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the actuators (e.g. turnouts) under the control of this controller
    ///
    /// @return     List of actuators
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::vector<layout::Actuator> getActuators () const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the routes configured for this controller. A route is a group of
    /// actuators and associated state that can be triggered
    ///
    /// @return     List of routes
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::vector<layout::Route> getRoutes () const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a route on  the controller
    ///
    /// @param[in]  name        Name of the route
    /// @param[in]  actuators   List of actuators and the state to set them to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    layout::Route createRoute (const std::string&               name,
                              const layout::routeList&         actuators);

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a new actuator
    ///
    /// @param[in]  name        Friendly name
    /// @param[in]  address     Track protocol address
    /// @param[in]  icon        UI Icon
    /// @param[in]  mode        Actuator mode
    /// @param[in]  duration    Actuation duration
    ///
    /// @return     Created actuator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    layout::Actuator createActuator (const std::string&     name,
                                     uint                   address,
                                     layout::actuatorIcon   icon,
                                     layout::actuatorMode   mode,
                                     uint                   duration);

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a locomotive
    ///
    /// @param[in]  name        Locomotive name
    /// @param[in]  proto       Track protocol
    /// @param[in]  address     Track protocol address
    /// @param[in]  functions   List of functions
    ///
    /// @return     Created locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    layout::Locomotive createLocomotive (const std::string&                     name,
                                         layout::trackProtocol                  proto,
                                         uint                                   address,
                                         const std::vector<layout::funcInfo>&   functions);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get a list of the items controlled by this controller than can be automated
    ///
    /// @return     List of automatable items
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::vector<AutomationItem> getAutomationItems () const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the health of the conenction to this controller
    ///
    /// @return     Controller connection health
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ConnectionWorkerThread::health getConnectionHealth () const
        { return m_thread.getConnectionHealth (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the meta class instance for this controller's type
    ///
    /// @return     Meta class instance
    ///
    /// @remarks    This is defined automatically by CONTROLLER_DEFINE().
    ///             Do not define manually.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual const ControllerMetaClassBase& getMetaClass () const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the meta class of the protocol used by this controller
    ///
    /// @return     Protocol meta class instance
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const control::ProtocolMetaClassBase& getProtocol () const { return m_thread.getProtocol (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the information for the device used to communicate with this controller
    /// (i.e. socket or serial port)
    ///
    /// @return     Device info
    ///
    ///////////////////////////////////////////////////////////////////////////////
    utils::device::deviceInfo getDeviceInfo () const { return m_thread.getDeviceInfo (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the friendly/UI name of this controller instance
    ///
    /// @return     Controller instance friendly name
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string getFriendlyName () const { return m_friendlyName; }

    //////////////////////////////////////////////////////////////////////////////
    /// Wait for the network thread to finish processing the queued messages
    ///
    //////////////////////////////////////////////////////////////////////////////
    void waitForMessages () const { m_thread.waitForNetworkQueue (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the capability controller, either hardware or software for this controller
    /// and a given capability
    ///
    /// @tparam     CapabilityController        Controller to get
    ///
    /// @return     Controller instance for capability
    ///
    //////////////////////////////////////////////////////////////////////////////
    template<class CapabiltyController>
    CapabiltyController* getCapabilityController ()
        {
        constexpr controllerCapability type = internal::capabilityTrait<CapabiltyController>::value;

        const auto&             meta        = getMetaClass ();
        CapabiltyController*    controller  = NULL;

        if (meta.hardwareCapabilities[type])
            {
            controller = ControllerMetaClassBase::cast<CapabiltyController> (this);
            }
        else if (meta.softwareCapabilities[type])
            {
            // TODO: virtual controllers
            }

        return controller;
        }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the capability controller, either hardware or software for this controller
    /// and a given capability. Const version
    ///
    /// @tparam     CapabilityController        Controller to get
    ///
    /// @return     Controller instance for capability
    ///
    //////////////////////////////////////////////////////////////////////////////
    template<class CapabiltyController>
    const CapabiltyController* getCapabilityController () const
        {
        return const_cast<control::ControllerBase*> (this)->getCapabilityController<CapabiltyController> ();
        }

protected:
    mutable ConnectionWorkerThread m_thread;    ///< Thread used to communicate with this controller

private:
    const std::string m_friendlyName;           ///< Name used in UI
    };
;

/// Information for creating a controller
struct createControllerInfo
    {
    std::string                 name;           ///< Controller type name
    std::string                 friendlyName;   ///< Controller instance friendly name
    std::string                 protocol;       ///< Protocol type string
    utils::device::deviceInfo   device;         ///< Communication device info
    };


///////////////////////////////////////////////////////////////////////////////
/// Create a controller
///
/// @param[in]  info        Creation info
///
///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<ControllerBase> createController (const createControllerInfo& info);


///////////////////////////////////////////////////////////////////////////////
/// Get a list of supported controllers
///
/// @return     List of controllers
///
///////////////////////////////////////////////////////////////////////////////
const std::vector<const ControllerMetaClassBase*> getControllers ();

} // namespace control
