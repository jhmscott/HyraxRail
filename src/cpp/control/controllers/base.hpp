/**
 * @file        controller/base.hpp
 * @brief       Abstract base class for all model train controllers,
 *              and metaclass utilitiess
 * @author      Justin Scott
 * @date        2026-01-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/worker.hpp>
#include <control/protocols/base.hpp>

#include <layout/actuator.hpp>
#include <layout/locomotive.hpp>
#include <layout/route.hpp>

///////////////////////////////////////////////////////////////////////////////
/// Add this macro the defintion of your controller class to associate with a meta class, giving this module
/// the ability to construct it programatically
///
/// @param[in]  type                      The controller type. This becoms the unique key for the controller
/// @param[in]  friendlyName    The name you wish to use for this controller in the UI
/// @param[in]  ...                         Variadic llist of protocol types this supports
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


// List of protocols supported  by a controller
using protocolMetaList = std::vector<const ProtocolMetaClassBase*>;

namespace internal
{

///////////////////////////////////////////////////////////////////////////////
/// Generates a list of the protocol meta-class objects from a varidac list of protocol types
///
/// @tparam     Protos...       Variadic list of protocol types
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

} // namespace internal

// Forward declare
class ControllerBase;


///////////////////////////////////////////////////////////////////////////////
/// Controller meta class abstract base class. Meta class is templated, so this provides a concrete type
/// with the meta class interface
///
///////////////////////////////////////////////////////////////////////////////
class ControllerMetaClassBase
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor. Inits the controlller type information
    ///
    /// @param[in]  name                      Configuration name of the controller. Used in the registry and to programatically
    ///                           construct. This must be unique and is typically created from the type name
    /// @param[in]  friendlyName    Name of the controller type to use in the UI
    /// @param[in]  protocols           List of protocols this controller supports
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ControllerMetaClassBase (const std::string&         name,
                             const std::string&         friendlyName,
                             const protocolMetaList&    protocols);

    const std::string       name;           ///< Type name (used internally)
    const std::string       friendlyName;   ///< Friendly name (used in UI)
    const protocolMetaList  protocols;      ///< List of protocols

    ///////////////////////////////////////////////////////////////////////////////
    /// Create an instance of this controller type
    ///
    /// @param[in]  friendlyName      Name of this controller instance to use in the UI
    /// @param[in]  protocol               Name of the protocol type
    /// @param[in]  info                        Used to create the device to communicate with the controller
    ///
    /// @return     unique_ptr instance of the requested controller
    ///             nullptr if the controller could not be created
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ControllerBase> create (const std::string&                  friendlyName,
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
    };


///////////////////////////////////////////////////////////////////////////////
/// Meta class derived type. Implements logic specific to the controller type
///
/// @tparam     T           Controller type. Must derive from ControllerBase
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
class ControllerMetaClass : public ControllerMetaClassBase
    {
public:
    // use base class constructor
    using ControllerMetaClassBase::ControllerMetaClassBase;


    ///////////////////////////////////////////////////////////////////////////////
    /// Create an instance of this controller type
    ///
    /// @param[in]  friendlyName      Name of this controller instance to use in the UI
    /// @param[in]  protocol               Name of the protocol type
    /// @param[in]  info                        Used to create the device to communicate with the controller
    ///
    /// @return     unique_ptr instance of the requested controller
    ///             nullptr if the controller could not be created
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ControllerBase> create (const std::string&                  friendlyName,
                                                    const std::string&                  protocol,
                                                    const utils::device::deviceInfo&    info) const override
        {
        static_assert (std::is_base_of_v<ControllerBase, T>, "Meta class must be used with a controller class");

        return std::make_unique<T> (friendlyName, findProtocol (protocol).create (info));
        }
    };


///////////////////////////////////////////////////////////////////////////////
/// Base class for all model train controller
///
/// @remarks    Derived classes must include CONTROLLER_DEFINE(...) within their defintion
///             to take advantage of the meta class features.
///
///////////////////////////////////////////////////////////////////////////////
class ControllerBase :
    public layout::LocomotiveController,
    public layout::ActuatorController,
    public layout::RouteController
    {
public:

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  friendlyName       Name of thi controller in UI
    /// @param[in]  proto                       Protocol to use. Controller takes ownership
    ///
    /// @remarks    Derived classes must have the same function signature for their constructors
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ControllerBase (const std::string& friendlyName, std::unique_ptr<ProtocolBase>&& proto);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the locomotives under the control of this controller
    ///
    /// @return     List of locomotives
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::vector<layout::Locomotive> getLocomotives () const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the actuators (e.g. turnouts) under the control of this controller
    ///
    /// @return     List of actuators
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::vector<layout::Actuator> getActuators () const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the routes configured for this controller. A route is a group of actuators and associated state that
    /// can be triggered
    ///
    /// @return     List of routes
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::vector<layout::Route> getRoutes () const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a route on  the controller
    ///
    /// @param[in]  name               Name of the route
    /// @param[in]  actuators   List of actuators and the state to set them to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual layout::Route createRoute (const std::string&                name,
                                       const layout::routeList&          actuators) = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the health of the conenction to this controller
    ///
    /// @return     Cotroller connnection health
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ConnectionWorkerThread::health getConnctionHealth () const
        { return m_thread.getConnectionHealth (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Trigger or leave an emergency stop state. This typically stops all locomotives by cutting track power
    ///
    /// @param[in]  stop            True to trigger an emergency stop
    ///                      False  to exit an emrgency stop state (sometimes called a go command)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void eStop (bool stop) = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if the controller is in the emergency stop state
    ///
    /// @return     True if the emergency stop state is active
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool isEStopped () = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the meta class instance for this controller's type
    ///
    /// @return     Meta class instance
    ///
    /// @remarks    This is defined automatically by CONTROLLER_DEFINE(). Do not define manually.
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
    /// Get the information for the device used to communicate with this controller (i.e. socket or serial port)
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
protected:
    mutable ConnectionWorkerThread m_thread;    ///< Thread used to communicate with this controller

private:
    const std::string m_friendlyName;           ///< Name used in UI
    };
;

// Information for creating a controller
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
