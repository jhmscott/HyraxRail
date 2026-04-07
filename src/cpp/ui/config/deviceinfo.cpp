/**
 * @file        config/deviceinfo.cpp
 * @brief       Defines widgets for entering transport layer device info
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/controllers/base.hpp>

#include <ui/common/utils.hpp>
#include <ui/config/deviceinfo.hpp>


namespace ui::config
{

NetworkDeviceInfoWidget::NetworkDeviceInfoWidget (QWidget* parent, utils::device::portNumber_t port) :
    DeviceInfoWidget (parent)
    {
    m_layout    = new QFormLayout{ this };

    m_network   = new QComboBox{ this };
    m_addressV4 = new common::IpV4Field{ this };
    m_addressV6 = new common::IpV6Field{ this };
    m_port      = new QLineEdit{ this };

    m_network->addItem ("IPv4", QVariant::fromValue (QHostAddress::IPv4Protocol));
    m_network->addItem ("IPv6", QVariant::fromValue (QHostAddress::IPv6Protocol));

    m_layout->addRow ("Network Protocol",   m_network);
    m_layout->addRow ("Address",            m_addressV4);
    m_layout->addRow ("Address",            m_addressV6);
    m_layout->addRow ("Port",               m_port);

    m_layout->setContentsMargins (0, 0, 0, 0);

    m_port->setValidator (new QIntValidator{ 1, 65535 });

    if (control::ProtocolMetaClassBase::NO_DEFAULT_PORT != port)
        {
        m_port->setText (QString::number (port));
        }

    networkProtoChanged (0);

    connect (m_network,
            &QComboBox::currentIndexChanged,
             this,
            &NetworkDeviceInfoWidget::networkProtoChanged);

    connect (m_addressV4,
            &common::IpField::inputChanged,
             this,
            &DeviceInfoWidget::inputChanged);

    connect (m_addressV6,
            &common::IpField::inputChanged,
             this,
            &DeviceInfoWidget::inputChanged);

    connect (m_port,
            &QLineEdit::textChanged,
             this,
            &NetworkDeviceInfoWidget::inputChanged);

    setLayout (m_layout);
    }

void NetworkDeviceInfoWidget::setInfo (utils::device::deviceInfo::info_t& info)
    {
    const utils::device::socketInfo& socket =
        std::get<utils::device::socketInfo> (info);

    common::setComboBoxIndexByUserData (*m_network, socket.addr.protocol ());

    if (QHostAddress::IPv4Protocol == socket.addr.protocol ())
        {
        m_addressV4->setValue (socket.addr);
        NetworkDeviceInfoWidget::networkProtoChanged (0);
        }
    else // (QHostAddress::IPv6Protocol == socket.addr.protocol ())
        {
        m_addressV6->setValue (socket.addr);
        NetworkDeviceInfoWidget::networkProtoChanged (1);
        }

    m_port->setText (QString::number (socket.port));
    }

utils::device::deviceInfo::info_t NetworkDeviceInfoWidget::getInfo () const
    {
    utils::device::socketInfo info;

    info.addr = m_activeIp->getValue ();
    info.port = m_port->text ().toInt ();

    return info;
    }

void NetworkDeviceInfoWidget::networkProtoChanged (int idx)
    {
    if (0 == idx)
        {
        m_layout->setRowVisible (m_addressV4, true);
        m_layout->setRowVisible (m_addressV6, false);
        m_activeIp = m_addressV4;
        }
    else
        {
        m_layout->setRowVisible (m_addressV4, false);
        m_layout->setRowVisible (m_addressV6, true);
        m_activeIp = m_addressV6;
        }

    emit inputChanged ();
    }

ComPortInfoWidget::ComPortInfoWidget (QWidget* parent) :
    DeviceInfoWidget (parent)
    {
    m_layout    = new QFormLayout{ this };

    m_comport   = new QComboBox{ this };
    m_baud      = new QComboBox{ this };

    for (auto& comPort : QSerialPortInfo::availablePorts ())
        {
        m_comport->addItem (comPort.portName (),
                            QVariant::fromValue (std::move (comPort)));
        }

    for (int baud : QSerialPortInfo::standardBaudRates ())
        {
        m_baud->addItem (QString::number (baud),
                         QVariant::fromValue (baud));
        }

    m_layout->addRow ("Baud Rate",  m_baud);
    m_layout->addRow ("COM Port",   m_comport);

    m_layout->setContentsMargins (0, 0, 0, 0);

    connect (m_baud,
            &QComboBox::currentIndexChanged,
             this,
            &DeviceInfoWidget::inputChanged);

    connect (m_comport,
            &QComboBox::currentIndexChanged,
             this,
            &DeviceInfoWidget::inputChanged);

    setLayout (m_layout);
    }

void ComPortInfoWidget::setInfo (utils::device::deviceInfo::info_t& info)
    {
    const utils::device::serialInfo& com = std::get<utils::device::serialInfo> (info);

    common::setComboBoxIndexByText (*m_comport, com.port.portName ());
    common::setComboBoxIndexByUserData (*m_baud, com.baud);
    }

utils::device::deviceInfo::info_t ComPortInfoWidget::getInfo () const
    {
    utils::device::serialInfo info;

    info.baud = m_baud->currentData ().toInt ();
    info.port = m_comport->currentData ().value<QSerialPortInfo> ();

    return info;
    }

}