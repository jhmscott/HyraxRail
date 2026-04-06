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

ProtocolBase::ProtocolBase (const utils::device::deviceInfo& deviceInfo, int timeout) :
    m_deviceInfo (deviceInfo),
    m_timeout (timeout)
    {
    if (false) // (not isDeviceSupported (*m_device, getMetaClass ().mask))
        {
        throw std::runtime_error ("IO Device not supported by protocol");
        }

    switch (deviceInfo.type)
        {
        case utils::device::TYPE_SERIAL:
            {
            auto com = std::get<utils::device::serialInfo> (m_deviceInfo.info);
            auto port = std::make_unique<QSerialPort> (com.port);

            port->setBaudRate (com.baud);

            m_device = std::move (port);
            break;
            }
        case utils::device::TYPE_TCP:
            {
            m_device = std::make_unique<QTcpSocket> ();
            break;
            }
        case utils::device::TYPE_UDP:
            {
            m_device = std::make_unique<QUdpSocket> ();
            break;
            }
        }
    }

bool ProtocolBase::maintainConnection ()
    {
    bool connected = true;

    switch (m_deviceInfo.type)
        {
        case utils::device::TYPE_UDP:
        case utils::device::TYPE_TCP:
            {
            QAbstractSocket* skt = static_cast<QAbstractSocket*> (m_device.get ());

            if (not (connected = skt->isValid ()))
                {
                auto& sktInfo = std::get<utils::device::socketInfo> (m_deviceInfo.info);

                skt->connectToHost (sktInfo.addr, sktInfo.port);
                connected = skt->waitForConnected ();
                }

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
        addr = std::get<utils::device::socketInfo> (m_deviceInfo.info).addr;
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

}