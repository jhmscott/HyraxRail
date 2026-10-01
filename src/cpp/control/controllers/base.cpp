/**
 * @file        controller/base.cpp
 * @brief       Abstract base class for all model train controllers,
 *              and metaclass utilitiess
 * @author      Justin Scott
 * @date        2026-02-01
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/controllers/base.hpp>

#include <map>

namespace control
{

//////////////////////////////////////////////////////////////////////////////
/// Determine the capabilities that can be emulated in software, given the
/// controller's hardware capabilities
///
/// @param[in]  hardware        Hardware capabilitier
///
/// @return     Software capabilities
///
//////////////////////////////////////////////////////////////////////////////
static controllerCapabilitySet getSoftwareCapabilities (const controllerCapabilitySet& hardware)
    {
    controllerCapabilitySet software{ 0 };

    if (hardware[CAPABILITY_ACTUATOR] && not hardware[CAPABILITY_ROUTE])
        {
        software[CAPABILITY_ROUTE] = true;
        }

    if (hardware[CAPABILITY_LOCOMOTIVE] && not hardware[CAPABILITY_ESTOP])
        {
        software[CAPABILITY_ESTOP] = true;
        }

    return software;
    }


ControllerMetaClassBase::ControllerMetaClassBase (const std::string&                name,
                                                  const std::string&                friendlyName,
                                                  const protocolMetaList&           protocols,
                                                  const controllerCapabilitySet&    capabilities) :
    name (name),
    friendlyName (friendlyName),
    protocols (protocols),
    hardwareCapabilities (capabilities),
    softwareCapabilities (getSoftwareCapabilities (capabilities))
    {
    controllerTypes.emplace (name, this);
    }

const ControllerMetaClassBase& ControllerMetaClassBase::fromController (const ControllerBase& controller)
    {
    return controller.getMetaClass ();
    }

const ProtocolMetaClassBase& ControllerMetaClassBase::findProtocol (const std::string& name) const
    {
    auto it = std::find_if (protocols.begin (),
                            protocols.end (),
                            [&name] (const ProtocolMetaClassBase* proto) -> bool
                            { return proto->name == name; });

    if (it == protocols.end ())
        {
        throw std::runtime_error ("Unknown protocol");
        }

    return **it;
    }

std::unique_ptr<ControllerBase> createController (const createControllerInfo& info)
    {
    std::unique_ptr<ControllerBase> controller = NULL;

    auto it = ControllerMetaClassBase::controllerTypes.find (info.name);

    if (ControllerMetaClassBase::controllerTypes.end () != it)
        {
        controller = it->second->create (info.friendlyName,
                                         info.protocol,
                                         info.device);
        }

    return controller;
    }

const std::vector<const ControllerMetaClassBase*> getControllers ()
    {
    using MetaPair = decltype (ControllerMetaClassBase::controllerTypes)::value_type;

    std::vector<const ControllerMetaClassBase*> controllers;

    controllers.reserve (ControllerMetaClassBase::controllerTypes.size ());

    std::transform (ControllerMetaClassBase::controllerTypes.begin (),
                    ControllerMetaClassBase::controllerTypes.end (),
                    std::back_inserter (controllers),
                    [] (const MetaPair& pair) -> const ControllerMetaClassBase*
                    { return pair.second; });

    return controllers;
    }

ControllerBase::ControllerBase (const std::string&              friendlyName,
                                std::unique_ptr<ProtocolBase>&& proto) :
    m_thread (friendlyName, std::move (proto)),
    m_friendlyName (friendlyName)
    {}

std::vector<layout::Locomotive> ControllerBase::getLocomotives () const
    {
    const auto* controller = getCapabilityController<layout::LocomotiveController> ();

    if (NULL != controller)
        {
        return controller->getLocomotives ();
        }
    else
        {
        return {};
        }
    }

std::vector<layout::Actuator> ControllerBase::getActuators () const
    {
    const auto* controller = getCapabilityController<layout::ActuatorController> ();

    if (NULL != controller)
        {
        return controller->getActuators ();
        }
    else
        {
        return {};
        }
    }

std::vector<layout::Route> ControllerBase::getRoutes () const
    {
    const auto* controller = getCapabilityController<layout::RouteController> ();

    if (NULL != controller)
        {
        return controller->getRoutes ();
        }
    else
        {
        return {};
        }
    }

layout::Route ControllerBase::createRoute (const std::string& name, const layout::routeList& actuators)
    {
    auto* controller = getCapabilityController<layout::RouteController> ();

    if (NULL != controller)
        {
        return controller->createRoute (name, actuators);
        }
    else
        {
        return {};
        }
    }

layout::Actuator ControllerBase::createActuator (const std::string&     name,
                                                 uint                   address,
                                                 layout::actuatorIcon   icon,
                                                 layout::actuatorMode   mode,
                                                 uint                   duration)
    {
    auto* controller = getCapabilityController<layout::ActuatorController> ();

    if (NULL != controller)
        {
        return controller->createActuator (name,
                                           address,
                                           icon,
                                           mode,
                                           duration);
        }
    else
        {
        return {};
        }
    }

layout::Locomotive ControllerBase::createLocomotive (const std::string&                     name,
                                                     layout::trackProtocol                  proto,
                                                     uint                                   address,
                                                     const std::vector<layout::funcInfo>&   functions)
    {
    auto* controller = getCapabilityController<layout::LocomotiveController> ();

    if (NULL != controller)
        {
        return controller->createLocomotive (name,
                                             proto,
                                             address,
                                             functions);
        }
    else
        {
        return {};
        }
    }

std::vector<AutomationItem> ControllerBase::getAutomationItems () const
    {
    std::vector<AutomationItem> items;

    auto actuators  = getActuators ();
    auto routes     = getRoutes ();
    auto locos      = getLocomotives ();


    size_t numFunction = std::accumulate (locos.begin (),
                                          locos.end (),
                                          0LLU,
        [] (size_t count, const layout::Locomotive& loco) -> size_t
        {
        return count + loco.getFunctions ().size ();
        });

    items.reserve (routes.size () + actuators.size () + numFunction);

    std::copy (actuators.begin (),
               actuators.end (),
               std::back_inserter (items));

    std::copy (routes.begin (),
               routes.end (),
               std::back_inserter (items));

    for (const layout::Locomotive& loco : locos)
        {
        auto functions = loco.getFunctions ();

        std::transform (functions.begin (),
                        functions.end (),
                        std::back_inserter (items),
                        [&loco] (const layout::funcInfo& info) -> AutomationItem
                        { return { loco, info.id }; });
        }

    return items;
    }

} // namespace control
