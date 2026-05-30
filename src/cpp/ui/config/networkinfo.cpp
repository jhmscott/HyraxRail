/**
 * @file        config/networkinfo.cpp
 * @brief       Form to enter network connection info
 * @author      Justin Scott
 * @date        2026-05-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/controllers/base.hpp>

#include <ui/config/networkinfo.hpp>
#include <ui/common/utils.hpp>

#include <utils/os.hpp>

namespace ui::config
{

static constexpr auto HOST_NAME_TYPE = QHostAddress::AnyIPProtocol;


NetworkDeviceInfoWidget::NetworkDeviceInfoWidget (QWidget* parent, utils::device::portNumber_t port) :
    DeviceInfoWidget (parent)
    {
    m_layout    = new QFormLayout{ this };

    m_network   = new QComboBox{ this };
    m_addressV4 = new common::IpV4Field{ this };
    m_addressV6 = new common::IpV6Field{ this };
    m_hostname  = new common::HostEntryField{ this };
    m_port      = new QLineEdit{ this };

    m_network->addItem (tr ("IPv4"),     QVariant::fromValue (QHostAddress::IPv4Protocol));

    if (utils::os::isIPv6Available ())
        {
        m_network->addItem (tr ("IPv6"), QVariant::fromValue (QHostAddress::IPv6Protocol));
        }

    m_network->addItem (tr ("Host Name"),QVariant::fromValue (HOST_NAME_TYPE));

    m_layout->addRow (new QLabel{ this }, m_network);
    m_layout->addRow (new QLabel{ this }, m_addressV4);
    m_layout->addRow (new QLabel{ this }, m_addressV6);
    m_layout->addRow (new QLabel{ this }, m_hostname);
    m_layout->addRow (new QLabel{ this }, m_port);

    setLabels ();

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

    connect (m_hostname,
            &common::HostEntryField::validityChanged,
             this,
            &DeviceInfoWidget::inputChanged);

    connect (m_port,
            &QLineEdit::textChanged,
             this,
            &NetworkDeviceInfoWidget::inputChanged);

    setLayout (m_layout);
    }

void NetworkDeviceInfoWidget::setInfo (const utils::device::deviceInfo::info_t& info)
    {
    const utils::device::socketInfo& socket =
        std::get<utils::device::socketInfo> (info);


    switch (socket.host.getType ())
        {
        case utils::device::HostInfo::type::HOSTNAME:
            {
            m_hostname->setText (socket.host.toString ());

            common::setComboBoxIndexByUserData (*m_network, HOST_NAME_TYPE);
            break;
            }
        case utils::device::HostInfo::type::IP:
            {
            QHostAddress    addr    = socket.host.toAddress ();
            auto            proto   = addr.protocol ();

            if (QHostAddress::IPv4Protocol == proto)
                {
                m_addressV4->setValue (addr);
                }
            else // (QHostAddress::IPv6Protocol == proto)
                {
                m_addressV6->setValue (addr);
                }

            common::setComboBoxIndexByUserData (*m_network, proto);
            break;
            }
        }

    networkProtoChanged (m_network->currentIndex ());

    m_port->setText (QString::number (socket.port));
    }

utils::device::deviceInfo::info_t NetworkDeviceInfoWidget::getInfo () const
    {
    utils::device::socketInfo info;

    info.host = m_activeIp->getHostInfo ();
    info.port = m_port->text ().toInt ();

    return info;
    }

bool NetworkDeviceInfoWidget::hasAcceptableInput () const
    {
    return m_port->hasAcceptableInput () &&
           m_activeIp->hasAcceptableInput ();
    }

QString NetworkDeviceInfoWidget::getErrorString () const
    {
    // boo using enum is c++ 20!
    using validity = common::HostEntryField::validity;

    QString error;

    if (HOST_NAME_TYPE == m_network->currentData ().value<QAbstractSocket::NetworkLayerProtocol> ())
        {
        switch (m_hostname->getValidityState ())
            {
            case validity::NON_RFC_1123_COMPLIANT:
                {
                error = tr ("Invalid host name format \"%1\"").arg (m_hostname->text ());
                break;
                }
            case validity::NON_EXISTENT_HOST:
                {
                error = tr ("Host \"%1\" does not exist").arg (m_hostname->text ());
                break;
                }
            }
        }

    return error;
    }

void NetworkDeviceInfoWidget::networkProtoChanged (int idx)
    {
    switch (m_network->itemData (idx).value<QAbstractSocket::NetworkLayerProtocol> ())
        {
        case QHostAddress::IPv4Protocol:
            {
            m_layout->setRowVisible (m_addressV4, true);
            m_layout->setRowVisible (m_addressV6, false);
            m_layout->setRowVisible (m_hostname, false);
            m_activeIp = m_addressV4;
            break;
            }
        case QHostAddress::IPv6Protocol:
            {
            m_layout->setRowVisible (m_addressV4, false);
            m_layout->setRowVisible (m_addressV6, true);
            m_layout->setRowVisible (m_hostname, false);
            m_activeIp = m_addressV6;
            break;
            }
        case HOST_NAME_TYPE:
            {
            m_layout->setRowVisible (m_addressV4, false);
            m_layout->setRowVisible (m_addressV6, false);
            m_layout->setRowVisible (m_hostname, true);
            m_activeIp = m_hostname;
            break;
            }
        }

    emit inputChanged ();
    }

void NetworkDeviceInfoWidget::setLabels ()
    {
    common::setFormRowText (*m_layout, *m_network,  tr ("Network Protocol"));
    common::setFormRowText (*m_layout, *m_addressV4,tr ("Address"));
    common::setFormRowText (*m_layout, *m_addressV6,tr ("Address"));
    common::setFormRowText (*m_layout, *m_hostname, tr ("Host Name"));
    common::setFormRowText (*m_layout, *m_port,     tr ("Port"));

    int idx = 0;

    m_network->setItemText (idx++, tr ("IPv4"));

    if (m_network->count () > 2)
        {
        m_network->setItemText (idx++, tr ("IPv6"));
        }

    m_network->setItemText (idx++, tr ("Host Name"));
    }
} // namespace ui::config