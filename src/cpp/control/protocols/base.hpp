/**
 * @file        proto/base.hpp
 * @brief       Abstract base class for representing
 *              the application layer for model train
 *              controller communication
 * @author      Justin Scott
 * @date        2026-01-04
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once


#include <common.hpp>
#include <utils/device.hpp>

#include <memory>

#define PROTOCOL_DEFINE(type, friendlyName, defaultType, defaultPort, ...) \
    public:\
        static const control::ProtocolMetaClassBase& getMetaClassStatic () { return meta; } \
        virtual const control::ProtocolMetaClassBase& getMetaClass () const override { return meta; } \
    private:\
        static inline const control::ProtocolMetaClass<type> meta{ #type, (friendlyName), (defaultType), (defaultPort), { __VA_ARGS__ } };


namespace control
{

class TimeoutError : public std::runtime_error
    {
public:
    explicit TimeoutError (int timeout) :
        std::runtime_error ("Operation timed out after "    +
                            std::to_string (timeout)        +
                            " ms"),
        m_timeout (timeout)
        {}

    int getTimeout () const { return m_timeout; }
private:
    int m_timeout;
    };


class ProtocolBase;


class ProtocolMetaClassBase
    {
public:
    static constexpr utils::device::portNumber_t NO_DEFAULT_PORT = 0;

    ProtocolMetaClassBase (const std::string&                       name,
                           const std::string&                       friendlyName,
                           const utils::device::type                defaultType,
                           const utils::device::portNumber_t        defaultPort,
                           const std::vector<utils::device::type>&  types) :
        name (name),
        friendlyName (friendlyName),
        defaultType (defaultType),
        defaultPort (defaultPort),
        types (types)
        {}


    const std::string                       name;
    const std::string                       friendlyName;
    const utils::device::type               defaultType;
    const utils::device::portNumber_t       defaultPort;
    const std::vector<utils::device::type>  types;

    virtual std::unique_ptr<ProtocolBase>
        create (const utils::device::deviceInfo& info) const = 0;
    };

template<class T>
class ProtocolMetaClass : public ProtocolMetaClassBase
    {
public:
    using ProtocolMetaClassBase::ProtocolMetaClassBase;

    virtual std::unique_ptr<ProtocolBase>
        create (const utils::device::deviceInfo& info) const override
        {
        return std::make_unique<T> (info);
        }
    };



class ProtocolBase
    {
public:
    ProtocolBase (const utils::device::deviceInfo& deviceInfo, int timeout);

    virtual ~ProtocolBase () {}

    bool maintainConnection ();

    bool isNetworkConnection ()
        { return utils::device::TYPE_SERIAL != m_deviceInfo.type; }

    QHostAddress getIpAddress ();

    virtual std::chrono::milliseconds getEventPollInterval () const
        { return std::chrono::milliseconds (1000); }


    virtual const control::ProtocolMetaClassBase& getMetaClass () const = 0;

    utils::device::deviceInfo getDeviceInfo () const { return m_deviceInfo; }

protected:
    std::unique_ptr<QIODevice> m_device;

    void waitForData ();

private:
    int                         m_timeout;   ///< in MS
    utils::device::deviceInfo   m_deviceInfo;

    };

}