/**
 * @file        controller/meta.cpp
 * @brief       Meta class for controller types
 * @author      Justin Scott
 * @date        2026-09-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/controllers/base.hpp>
#include <control/controllers/meta.hpp>

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

    utils::algorithm::forEachType<softwareControllerTypes> (
        [&] (auto envelope)
        {
        using VirtualController     = typename decltype (envelope)::type;
        using Dependencies          = typename traits::virtualController<VirtualController>::dependencies;
        using CapabilityController  = typename traits::virtualController<VirtualController>::base;

        constexpr controllerCapability type = traits::capability<CapabilityController>::value;

        if (not hardware[type])
            {
            bool hasDependencies = true;

            utils::algorithm::forEachType<Dependencies> (
                [&] (auto dependency)
                {
                using Dependency = typename decltype (dependency)::type;

                constexpr controllerCapability dependentType = traits::capability<Dependency>::value;

                hasDependencies = hasDependencies && hardware[dependentType];

                });

            software[type] = hasDependencies;
            }
        });

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
}