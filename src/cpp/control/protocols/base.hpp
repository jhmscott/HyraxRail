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

///////////////////////////////////////////////////////////////////////////////
/// Add this macro the defintion of your protocol class to associate with a meta class, giving this module
/// the ability to construct it programatically
///
/// @param[in]  type                      The controller type. This becoms the unique key for the controller
/// @param[in]  friendlyName    The name you wish to use for this controller in the UI
/// @param[in]  defaultType       Default device type (utils::device::type)
/// @param[in]  defaultPort       Default port number. Use NO_DEFAULT_PORT to not populate this
///                           automatically in the UI. N/A for COM porte
/// @param[in]  ...                         Variadic llist of supported device types (utils::device::type)
///
///////////////////////////////////////////////////////////////////////////////
#define PROTOCOL_DEFINE(type, friendlyName, defaultType, defaultPort, ...) \
    public:\
        static const control::ProtocolMetaClassBase& getMetaClassStatic () { return meta; } \
        virtual const control::ProtocolMetaClassBase& getMetaClass () const override { return meta; } \
    private:\
        static inline const control::ProtocolMetaClass<type> meta{ #type, (friendlyName), (defaultType), (defaultPort), { __VA_ARGS__ } };


namespace control
{
///////////////////////////////////////////////////////////////////////////////
/// Connection timeout error
///
///////////////////////////////////////////////////////////////////////////////
class TimeoutError : public std::runtime_error
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  timeout     Milliseconds ellapsed before timeout
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit TimeoutError (int timeout) :
        std::runtime_error ("Operation timed out after "    +
                            std::to_string (timeout)        +
                            " ms"),
        m_timeout (timeout)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the time ellapsed before timrout
    ///
    /// @return     TIme in milliseconds
    ///
    ///////////////////////////////////////////////////////////////////////////////
    int getTimeout () const { return m_timeout; }
private:
    int m_timeout;  ///< Millisecons before timeout occured
    };

// Forward declare
class ProtocolBase;

///////////////////////////////////////////////////////////////////////////////
/// Protocol meta class base. Meta class is templated as it has type dependent logic. This provides a
/// concrete type we can store pointers to
///
///////////////////////////////////////////////////////////////////////////////
class ProtocolMetaClassBase
    {
public:
    static constexpr utils::device::portNumber_t NO_DEFAULT_PORT = 0;   ///< Use when no default port should be provided in the UI

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  name                        Protocol type name. Must be unique
    /// @param[in]  friendlyName      Name to use for this protocol in the UI
    /// @param[in]  defaultType        Default device type
    /// @param[in]  defaultPort        Default port number. Use NO_DEFAULT_PORT to not populate this
    ///                            automatically in the UI. N/A for COM porte
    /// @param[in]  types                      List of supported device types
    ///
    ///////////////////////////////////////////////////////////////////////////////
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


    const std::string                       name;           ///< Protocol type name. Must be unique
    const std::string                       friendlyName;   ///< Name to use for this protocol in the UI
    const utils::device::type               defaultType;    ///< Default device type
    const utils::device::portNumber_t       defaultPort;    ///< Default port number
    const std::vector<utils::device::type>  types;          ///< List of supported types
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Create an instance of this protocol type
    ///
    /// @param[in]  info        Connection device info
    ///
    /// @return     Protocol instance
    ///             nullptr if the protocol could not be created
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ProtocolBase>
        create (const utils::device::deviceInfo& info) const = 0;
    };

///////////////////////////////////////////////////////////////////////////////
/// Protocol meta clas type. Derived and templated version
///
/// @tparam     T           Protocol type
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
class ProtocolMetaClass : public ProtocolMetaClassBase
    {
public:
    // use base class constructor
    using ProtocolMetaClassBase::ProtocolMetaClassBase;

    ///////////////////////////////////////////////////////////////////////////////
    /// Create an instance of this protocol type
    ///
    /// @param[in]  info        Connection device info
    ///
    /// @return     Protocol instance
    ///             nullptr if the protocol could not be created
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<ProtocolBase>
        create (const utils::device::deviceInfo& info) const override
        {
        return std::make_unique<T> (info);
        }
    };

///////////////////////////////////////////////////////////////////////////////
/// Base class for all controller connection protocols
///
/// @remarks    Add PROTOCOL_DEFINE() to your derived class defintion
///
///////////////////////////////////////////////////////////////////////////////
class ProtocolBase
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  deviceInfo      Connection device info
    /// @param[in]  timeout             Time before connection times out
    ///
    /// @remarks    Derived classes must have a constructor that takes just the deviceInfo parameter
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ProtocolBase (const utils::device::deviceInfo& deviceInfo, int timeout);
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Virtual destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~ProtocolBase () {}
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Check if the connection is still open and re-open if necassary
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool maintainConnection ();
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Check if this is a network (i,e. socket) connection
    ///
    /// @return     true if this a netowkr connection
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool isNetworkConnection ()
        { return utils::device::TYPE_SERIAL != m_deviceInfo.type; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the IP address of the connection. Undefined for COM port connections
    ///
    /// @return     IP Addres
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QHostAddress getIpAddress ();
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the interval to poll for events
    ///
    /// @return     polling intervals
    ///
    /// @remarks    Defaults to 1000 ms. Override this in your base class if you need a different or dynamic
    ///             value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::chrono::milliseconds getEventPollInterval () const
        { return std::chrono::milliseconds (1000); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get this protocol type's metaclass
    ///
    /// @return     Meta class instance
    ///
    /// @remarks    This is implement automatically by PROTOCOL_DEFINE(). Do not implement manually.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual const control::ProtocolMetaClassBase& getMetaClass () const = 0;
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the connection device info for this protocol's instance
    ///
    /// @return     Device info
    ///
    ///////////////////////////////////////////////////////////////////////////////
    utils::device::deviceInfo getDeviceInfo () const { return m_deviceInfo; }

protected:
    std::unique_ptr<QIODevice> m_device;        ///< IO Device used to communicate with controller
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Wait for new data from the device
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void waitForData ();

private:
    int                         m_timeout;      ///< Timeout in MS
    utils::device::deviceInfo   m_deviceInfo;   ///< Device info

    };

}
