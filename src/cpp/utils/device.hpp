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

namespace utils
{

namespace device
{


enum type
    {
    TYPE_UDP,                          ///< UDP Socket (QUdpSocket)
    TYPE_TCP,                          ///< TCP Socket (QTcpSocket
    TYPE_SERIAL,                       ///< Serial/COM Port (QSerialPort)

    TYPE_UNSUPPORTED,                  ///< Unsupported device type
                                            ///  Keep this after all supported types

    NUM_TYPES = TYPE_UNSUPPORTED  ///< Delimiter only, number of supported types
    };

inline constexpr const char* typeNames[] =
    {
    "UDP",
    "TCP",
    "Serial",
    };


using mask = std::bitset<NUM_TYPES>;


using portNumber_t = uint16_t;

struct socketInfo
    {
    QHostAddress    addr;
    portNumber_t    port;
    };

struct serialInfo
    {
    QSerialPortInfo port;
    int             baud;
    };

struct deviceInfo
    {
    using info_t = std::variant<socketInfo, serialInfo>;

    type    type;
    info_t  info;
    };



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

constexpr bool isDeviceSupported (type type, const mask& mask)
    {
    bool supported = false;

    if (TYPE_UNSUPPORTED != type)
        {
        supported = mask[type];
        }

    return supported;
    }

inline bool isDeviceSupported (const QIODevice& device, const mask& mask)
    {
    return isDeviceSupported (getDeviceType (device), mask);
    }

template<class... Args>
constexpr mask createMask (Args... args)
    {
    // static_assert (std::is_same_v<Args, deviceType> || ...,
   //                    "Arguments must be device types");

    mask mask{ 0 };

    // mask[args] = true, ...;

    return mask;
    }

}
}