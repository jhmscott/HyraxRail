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


#define CONTROLLER_DEFINE(type, friendlyName, ...) \
    public:\
        static const control::ControllerMetaClassBase& getMetaClassStatic () { return meta; } \
        virtual const control::ControllerMetaClassBase& getMetaClass () const override { return meta; } \
    private:\
        static inline const control::ControllerMetaClass<type> meta{ #type, (friendlyName), control::internal::makeProtocolList<__VA_ARGS__> () };

namespace control
{
using protocolMetaList = std::vector<const ProtocolMetaClassBase*>;

namespace internal
{

template<class... Protos>
protocolMetaList makeProtocolList ()
    {
    static_assert (sizeof...(Protos) > 0, "Must pass at least one protocol");

    return { (&Protos::getMetaClassStatic ())...};
    }

};

class ControllerBase;

class ControllerMetaClassBase
    {
public:
    explicit ControllerMetaClassBase (const std::string& name,
                                      const std::string& friendlyName,
                                      const protocolMetaList& protocols);

    const std::string       name;             ///< Type name (used internally)
    const std::string       friendlyName;     ///< Friendly name (used in UI)
    const protocolMetaList  protocols;

    virtual std::unique_ptr<ControllerBase> create (const std::string&                  friendlyName,
                                                    const std::string&                  protocol,
                                                    const utils::device::deviceInfo&    info) const = 0;

    const ProtocolMetaClassBase& findProtocol (const std::string& protocol) const;
    };

template<class T>
class ControllerMetaClass : public ControllerMetaClassBase
    {
public:
    using ControllerMetaClassBase::ControllerMetaClassBase;

    virtual std::unique_ptr<ControllerBase> create (const std::string&                  friendlyName,
                                                    const std::string&                  protocol,
                                                    const utils::device::deviceInfo&    info) const override
        {
        return std::make_unique<T> (friendlyName, findProtocol (protocol).create (info));
        }
    };


class ControllerBase :
    public layout::LocomotiveController,
    public layout::ActuatorController,
    public layout::RouteController
    {
public:
    ControllerBase (const std::string& friendlyName, std::unique_ptr<ProtocolBase>&& proto);

    virtual std::vector<layout::Locomotive> getLocomotives () const = 0;

    virtual std::vector<layout::Actuator> getActuators () const = 0;

    virtual std::vector<layout::Route> getRoutes () const = 0;

    ConnectionWorkerThread::health getConnctionHealth () const
        { return m_thread.getConnectionHealth (); }

    virtual void eStop (bool stop) = 0;

    virtual bool isEStopped () = 0;

    virtual const ControllerMetaClassBase& getMetaClass () const = 0;

    const control::ProtocolMetaClassBase& getProtocol () const { return m_thread.getProtocol (); }

    utils::device::deviceInfo getDeviceInfo () const { return m_thread.getDeviceInfo (); }

    std::string getFriendlyName () const { return m_friendlyName; }
protected:
    mutable ConnectionWorkerThread m_thread;

private:
    const std::string m_friendlyName;
    };

using controllerOwnerList   = std::vector<std::unique_ptr<ControllerBase>>;
using controllerList        = std::vector<ControllerBase*>;

struct createControllerInfo
    {
    std::string                 name;
    std::string                 friendlyName;
    std::string                 protocol;
    utils::device::deviceInfo   device;
    };


std::unique_ptr<ControllerBase> createController (const std::string&                name,
                                                  const std::string&                friendlyName,
                                                  const std::string&                protocol,
                                                  const utils::device::deviceInfo&  info);

inline std::unique_ptr<ControllerBase> createController (const createControllerInfo& info)
    {
    return createController (info.name, info.friendlyName, info.protocol, info.device);
    }

const std::vector<const ControllerMetaClassBase*> getControllers ();
}
