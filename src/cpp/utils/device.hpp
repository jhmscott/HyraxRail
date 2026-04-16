/**
 * @file        utils/device.hpp
 * @brief       Helper functions and defintions for transport layer
 *              devices (i.e. sockets and com ports)
 * @author      Justin Scott
 * @date        2026-01-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */



#pragma once

#include <bitset>
#include <variant>

#include <QIODevice>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace utils::device
{


// device type
enum type
    {
    TYPE_UDP,                           ///< UDP Socket (QUdpSocket)
    TYPE_TCP,                           ///< TCP Socket (QTcpSocket
    TYPE_SERIAL,                        ///< Serial/COM Port (QSerialPort)

    TYPE_UNSUPPORTED,                   ///< Unsupported device type
                                        ///  Keep this after all supported types

    NUM_TYPES = TYPE_UNSUPPORTED        ///< Delimiter only, number of supported types
    };

// Device type names for use in the UI
inline constexpr const char* typeNames[] =
    {
    "UDP",
    "TCP",
    "Serial",
    };

// Device type mask. Used to indicate a set of types, typically supported types
using mask = std::bitset<NUM_TYPES>;

// TCP/UDP port number, 16-bits
using portNumber_t = uint16_t;

// Information needed to bind to a socket
struct socketInfo
    {
    QHostAddress    addr;       ///< IPv4/IPv6 address
    portNumber_t    port;       ///< TCP/UDP port number
    };

// Information needed to bind to a serial/COM port
struct serialInfo
    {
    QSerialPortInfo port;       ///< COM port
    int             baud;       ///< Baud rate (bits/second)
    };

struct deviceInfo
    {
    // variant, determined by type member
    using info_t = std::variant<socketInfo, serialInfo>;

    type    type;       ///< Device type
    info_t  info;       ///< Device info
                        ///  socketInfo if type == TYPE_TCP or type == TYPE_UDP
                        ///  serialInfo   if type == TYPE_SERIAL
    };

///////////////////////////////////////////////////////////////////////////////
/// Get the device type from a QT I/O device
///
/// @param[in]  device          I/O device
///
/// @return     Enumerated device type. TYPE_UNSUPPORTED if not a supported device type
///
///////////////////////////////////////////////////////////////////////////////
inline type getDeviceType (const QIODevice& device)
    {
    const QIODevice* const  pDevice = &device;
    type                    type    = TYPE_UNSUPPORTED;

    if (NULL != qobject_cast<const QUdpSocket*> (pDevice))
        {
        type = TYPE_UDP;
        }
    else if (NULL != qobject_cast<const QTcpSocket*> (pDevice))
        {
        type = TYPE_TCP;
        }
    else if (NULL != qobject_cast<const QSerialPort*> (pDevice))
        {
        type = TYPE_SERIAL;
        }

    return type;
    }

///////////////////////////////////////////////////////////////////////////////
/// Check if device is enabled in a bitset
///
/// @param[in]  type        Enumerated device type
/// @param[in]  mask        Device type mask
///
/// @return     true if type is enabled
///
///////////////////////////////////////////////////////////////////////////////
constexpr bool isDeviceSupported (type type, const mask& mask)
    {
    bool supported = false;

    if (TYPE_UNSUPPORTED != type)
        {
        supported = mask[type];
        }

    return supported;
    }

///////////////////////////////////////////////////////////////////////////////
/// Check if a device is one of the supported types in a mask
///
/// @param[in]  device          Device to check
/// @param[in]  mask               Device type mask
///
/// @return     true is device is of an enabled type
    /// 
///////////////////////////////////////////////////////////////////////////////
inline bool isDeviceSupported (const QIODevice& device, const mask& mask)
    {
    return isDeviceSupported (getDeviceType (device), mask);
    }

// TODO: Get this working
template<class... Args>
constexpr mask createMask (Args... args)
    {
    // static_assert (std::is_same_v<Args, deviceType> || ...,
   //                    "Arguments must be device types");

    mask mask{ 0 };

    // mask[args] = true, ...;

    return mask;
    }

} // namespace utils::device
