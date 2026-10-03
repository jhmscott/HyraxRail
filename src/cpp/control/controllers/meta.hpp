/**
 * @file        controller/meta.hpp
 * @brief       Meta class for controller types
 * @author      Justin Scott
 * @date        2026-09-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/utils.hpp>

#include <control/protocols/base.hpp>

#include <layout/actuator.hpp>
#include <layout/estop.hpp>
#include <layout/locomotive.hpp>
#include <layout/route.hpp>

namespace control
{
// Forward declare
class ControllerBase;
struct createControllerInfo;
std::unique_ptr<ControllerBase> createController (const createControllerInfo& info);


/// List of protocols supported  by a controller
using protocolMetaList          = std::vector<const ProtocolMetaClassBase*>;


/// internal metaclass controller namespace
namespace controllermeta_internal
{

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

    utils::algorithm::forEachType<capabilityControllerTypes> (
        [&] (auto envelope)
        {
        using Capability = typename decltype (envelope)::type;

        if constexpr (std::is_base_of_v<Capability, Controller>)
            {
            capabilities[traits::capability<Capability>::value] = true;
            }
        });

    return capabilities;
    }
} // namespace controllermeta_internal


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
        const auto& self = fromController (*controller);
        void*       vptr = self.asCapabilityController (controller,
                                                        typeid (Capability));

        return static_cast<Capability*> (vptr);
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
    /// Cast implementation of controller base -> capability controller
    ///
    /// @param[in]  controller      Controller base
    /// @param[in]  info            Type info for the capability controller to get
    ///
    /// @return     Capability controller if controller has that capability
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void* asCapabilityController (ControllerBase*       controller,
                                          const std::type_info& info) const = 0;

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
        ControllerMetaClassBase (name, friendlyName, protocols, controllermeta_internal::deduceHardwareCapabilites<T> ())
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
    /// Cast implementation of controller base -> capability controller
    ///
    /// @param[in]  controller      Controller base
    /// @param[in]  info            Type info for the capability controller to get
    ///
    /// @return     Capability controller if controller has that capability
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void* asCapabilityController (ControllerBase*       controller,
                                          const std::type_info& info) const override
        {
        void* capability = NULL;

        utils::algorithm::forEachType<capabilityControllerTypes> (
            [&] (auto envelope)
            {
            using Controller = typename decltype (envelope)::type;

            if constexpr (std::is_base_of_v<Controller, T>)
                {
                if (typeid (Controller) == info)
                    {
                    capability = static_cast<Controller*> (
                                    static_cast<T*> (controller));
                    }
                }
            });

        return capability;
        }
    };


///////////////////////////////////////////////////////////////////////////////
/// Get a list of supported controllers
///
/// @return     List of controllers
///
///////////////////////////////////////////////////////////////////////////////
const std::vector<const ControllerMetaClassBase*> getControllers ();
}