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

#include <layout/virtual/vestop.hpp>

#include <map>

namespace control
{


//////////////////////////////////////////////////////////////////////////////
/// Create a software capability for a hardware controller. Implementation
///
/// @tparam     VirtualController   Controller to create controller for
/// @tparam     Dependencies        Dependent controller types
///
/// @param[in]  controller          Controller to create software capability for
/// @param[in]  unused              Tuple type to pass dependent types
///
/// @return     Created capability
///
//////////////////////////////////////////////////////////////////////////////
template<class VirtualController, class Dependencies, size_t... Is>
static std::unique_ptr<layout::VirtualControllerBase>
createSoftwareCapability (control::ControllerBase&      controller,
                          std::index_sequence<Is...>    is)
    {
    auto* capability = new VirtualController{
        *ControllerMetaClassBase::cast<typename std::tuple_element_t<Is, Dependencies>> (&controller)... };

    return std::unique_ptr<layout::VirtualControllerBase>{ capability };
    }

//////////////////////////////////////////////////////////////////////////////
/// Create a software capability for a hardware controller
///
/// @tparam     VirtualController   Controller to create controller for
///
/// @param[in]  controller          Controller to create software capability for
///
/// @return     Created capability
///
//////////////////////////////////////////////////////////////////////////////
template<class VirtualController>
static std::unique_ptr<layout::VirtualControllerBase>
createSoftwareCapability (control::ControllerBase& controller)
    {
    using Dependencies  = typename traits::virtualController<VirtualController>::dependencies;
    constexpr size_t size = std::tuple_size_v<Dependencies>;

    return createSoftwareCapability<VirtualController, Dependencies> (controller, std::make_index_sequence<size>{});
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

void ControllerBase::initSoftwareCapabilitiesOnce ()
    {
    std::call_once (m_swInitOnce, &ControllerBase::initSoftwareCapabilities, this);
    }

void ControllerBase::initSoftwareCapabilities ()
    {
    const auto& meta = getMetaClass ();

    utils::algorithm::forEachType<softwareControllerTypes> (
        [&] (auto envelope)
        {
        using VirtualController     = typename decltype (envelope)::type;
        using CapabilityController  = typename traits::virtualController<VirtualController>::base;

        constexpr controllerCapability type = traits::capability<CapabilityController>::value;

        m_softwareControllers[type] = createSoftwareCapability<VirtualController> (*this);
        });
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

} // namespace control
