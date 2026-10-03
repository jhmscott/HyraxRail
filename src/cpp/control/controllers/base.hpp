/**
 * @file        controller/base.hpp
 * @brief       Abstract base class for all model train controllers
 * @author      Justin Scott
 * @date        2026-01-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/item.hpp>
#include <control/controllers/worker.hpp>
#include <control/controllers/meta.hpp>
#include <control/protocols/base.hpp>

#include <layout/actuator.hpp>
#include <layout/estop.hpp>
#include <layout/locomotive.hpp>
#include <layout/route.hpp>

#include <layout/virtual/base.hpp>


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
        static inline const control::ControllerMetaClass<type> meta{ #type, (friendlyName), control::controllerbase_internal::makeProtocolList<__VA_ARGS__> () };

namespace control
{

/// internal base controller namespace
namespace controllerbase_internal
{



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

    return { (&Protos::getMetaClassStatic ())... };
    }

} // namespace controllerbase_internal



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
    layout::Route createRoute (const std::string&       name,
                               const layout::routeList& actuators);

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
        using namespace controllerbase_internal;

        constexpr controllerCapability type = traits::capability<CapabiltyController>::value;

        const auto&             meta        = getMetaClass ();
        CapabiltyController*    controller  = NULL;

        if (meta.hardwareCapabilities[type])
            {
            controller = ControllerMetaClassBase::cast<CapabiltyController> (this);
            }
        else if (meta.softwareCapabilities[type])
            {
            initSoftwareCapabilitiesOnce ();
            controller = dynamic_cast<CapabiltyController*> (m_softwareControllers[type].get ());
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
    mutable ConnectionWorkerThread m_thread;        ///< Thread used to communicate with this controller

private:
    /// Pointer to a virtual (software implemented) capability controller
    using softwareController        = std::unique_ptr<layout::VirtualControllerBase>;

    /// List of controllers implemented in software
    using softwareCapabilityList    = std::array<softwareController, NUM_CAPABILITIES>;

    softwareCapabilityList  m_softwareControllers;  ///< Controller capabilities implemented in software
    const std::string       m_friendlyName;         ///< Name used in UI
    std::once_flag          m_swInitOnce;           ///< Once flag for the software capability init

    void initSoftwareCapabilitiesOnce ();

    void initSoftwareCapabilities ();
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

} // namespace control
