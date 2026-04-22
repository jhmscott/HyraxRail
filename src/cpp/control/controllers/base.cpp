/**
 * @file        controller/base.cpp
 * @brief       Abstract base class for all model train controllers,
 *              and metaclass utilitiess
 * @author      Justin Scott
 * @date        2026-02-01
 *
 * @copyright   Copyright (c) 2026 Justin ScottS
 */


#include <control/controllers/base.hpp>

#include <map>

namespace control
{
static std::map<std::string, ControllerMetaClassBase*> controllerTypes; ///< Supported controller types
                                                                        ///  populated automatically by metaclass
                                                                        ///  global constructors



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
                            [&name] (const  ProtocolMetaClassBase* proto)
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

    auto it = controllerTypes.find (info.name);

    if (controllerTypes.end () != it)
        {
        controller = it->second->create (info.friendlyName, info.protocol, info.device);
        }

    return controller;
    }

const std::vector<const ControllerMetaClassBase*> getControllers ()
    {
    std::vector<const ControllerMetaClassBase*> controllers;

    controllers.reserve (controllerTypes.size ());

    std::transform (controllerTypes.begin (),
                    controllerTypes.end (),
                    std::back_inserter (controllers),
                    [] (std::pair<std::string, ControllerMetaClassBase*> pair)
                    { return pair.second; });

    return controllers;
    }

ControllerBase::ControllerBase (const std::string&              friendlyName,
                                std::unique_ptr<ProtocolBase>&& proto) :
    m_thread (friendlyName, std::move (proto)),
    m_friendlyName (friendlyName)
    {
    }

} // namespace control
