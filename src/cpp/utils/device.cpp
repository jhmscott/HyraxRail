/**
 * @file        utils/device.cpp
 * @brief       Helper functions and defintions for transport layer
 *              devices (i.e. sockets and com ports)
 * @author      Justin Scott
 * @date        2026-05-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/device.hpp>
#include <utils/os.hpp>

#include<QHostInfo>

namespace utils::device
{
QHostAddress HostInfo::toAddress () const
    {
    QHostAddress addr;

    switch (getType ())
        {
        case type::IP:
            {
            addr = std::get<QHostAddress> (m_value);
            break;
            }
        case type::HOSTNAME:
            {
            QHostInfo   hi          = QHostInfo::fromName (std::get<QString> (m_value));
            auto        addresses   = hi.addresses ();
            auto        firstV4     =
                std::find_if (addresses.begin (),
                              addresses.end (),
                [] (const QHostAddress& addr) -> bool
                {
                return QHostAddress::IPv4Protocol == addr.protocol ();
                });

            auto        firstV6     = addresses.end ();

            // Only consider IPv6 addresses if available on this machine
            if (os::isIPv6Available ())
                {
                firstV6 =
                    std::find_if (addresses.begin (),
                                  addresses.end (),
                    [] (const QHostAddress& addr) -> bool
                    {
                    return QHostAddress::IPv6Protocol == addr.protocol ();
                    });
                }

            // Prefer IPv6 if it's available
            if (addresses.end () != firstV6)
                {
                addr = *firstV6;
                }
            else if (addresses.end () != firstV4)
                {
                addr = *firstV4;
                }

            break;
            }
        }

    return addr;
    }

QString HostInfo::toString () const
    {
    QString string;

    switch (getType ())
        {
        case type::IP:
            {
            string = std::get<QHostAddress> (m_value).toString ();
            break;
            }
        case type::HOSTNAME:
            {
            string = std::get<QString> (m_value);
            break;
            }
        }

    return string;
    }

HostInfo HostInfo::fromString (const QString& string, type type)
    {
    HostInfo info;

    switch (type)
        {
        case type::IP:
            {
            info.m_value = QHostAddress{ string };
            break;
            }

        case type::HOSTNAME:
            {
            info.m_value = string;
            break;
            }
        }

    return info;
    }

} // namespace utils::device