/**
 * @file        proto/base.cpp
 * @brief       Abstract base class for representing
 *              the application layer for model train
 *              controller communication
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/protocols/base.hpp>

namespace control
{

///////////////////////////////////////////////////////////////////////////////
/// Create a device
///
/// @param[in]  deviceInfo  Device configuration info
///
/// @return    Created device
///
///////////////////////////////////////////////////////////////////////////////
static std::unique_ptr<QIODevice> createDevice (const utils::device::deviceInfo& deviceInfo)
    {
    std::unique_ptr<QIODevice> device;

    switch (deviceInfo.type)
        {
        case utils::device::TYPE_SERIAL:
            {
            auto com  = std::get<utils::device::serialInfo> (deviceInfo.info);
            auto port = std::make_unique<QSerialPort> (com.port);

            port->setBaudRate (com.baud);

            device = std::move (port);
            break;
            }
        case utils::device::TYPE_TCP:
            {
            device = std::make_unique<QTcpSocket> ();
            break;
            }
        case utils::device::TYPE_UDP:
            {
            device = std::make_unique<QUdpSocket> ();
            break;
            }
        default:
            {
            throw std::runtime_error ("Invalide device type");
            break;
            }
        }

    return device;
    }


ProtocolBase::ProtocolBase (const utils::device::deviceInfo& deviceInfo, int timeout) :
    m_deviceInfo (deviceInfo),
    m_timeout (timeout)
    {
    if (false) // (not isDeviceSupported (*m_device, getMetaClass ().mask))
        {
        throw std::runtime_error ("IO Device not supported by protocol");
        }

    m_device = createDevice (deviceInfo);
    }

bool ProtocolBase::maintainConnection ()
    {
    bool connected = false;

    switch (m_deviceInfo.type)
        {
        case utils::device::TYPE_UDP:
        case utils::device::TYPE_TCP:
            {
            QAbstractSocket* skt = static_cast<QAbstractSocket*> (m_device.get ());

            if (QAbstractSocket::ConnectedState == skt->state ())
                {
                connected = true;
                }
            else
                {
                auto& sktInfo = std::get<utils::device::socketInfo> (m_deviceInfo.info);

                skt->connectToHost (sktInfo.host.toAddress (),
                                    sktInfo.port);
                if (not (connected = skt->waitForConnected (1000)))
                    {
                    skt->disconnect ();
                    }
                }

            break;
            }
        default:
            {
            connected = true;
            break;
            }
        }

    return connected;
    }

QHostAddress ProtocolBase::getIpAddress ()
    {
    QHostAddress addr{};

    if (isNetworkConnection ())
        {
        addr = std::get<utils::device::socketInfo>
                    (m_deviceInfo.info).host.toAddress ();
        }

    return addr;
    }

void ProtocolBase::waitForData ()
    {
    if (!m_device->waitForReadyRead (m_timeout))
        {
        throw TimeoutError (m_timeout);
        }
    }

void ProtocolBase::setDeviceInfo (const utils::device::deviceInfo& info)
    {
    {
    std::lock_guard lk{ m_deviceLock };
    m_deviceInfo = info;
    }

    m_device = createDevice (info);
    }
}