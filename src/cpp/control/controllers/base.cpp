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



ControllerMetaClassBase::ControllerMetaClassBase (const std::string&        name,
                                                  const std::string&        friendlyName,
                                                  const protocolMetaList&   protocols) :
    name (name),
    friendlyName (friendlyName),
    protocols (protocols)
    {
    controllerTypes.emplace (name, this);
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

std::vector<AutomationItem> ControllerBase::getAutomationItems () const
    {
    std::vector<AutomationItem> items;

    auto actuators  = getActuators ();
    auto routes     = getRoutes ();

    items.reserve (routes.size () + actuators.size ());

    std::copy (actuators.begin (),
               actuators.end (),
               std::back_inserter (items));

    std::copy (routes.begin (),
               routes.end (),
               std::back_inserter (items));

    return items;
    }

} // namespace control
