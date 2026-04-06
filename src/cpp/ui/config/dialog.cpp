/**
 * @file        config/dialog.cpp
 * @brief       Dialog box for the configuration of controllers
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/config/dialog.hpp>


#include <QDialogButtonBox>

namespace ui::config
{

class DeviceInfoWidget : public QWidget
    {
public:
    explicit DeviceInfoWidget (QWidget* parent) :
        QWidget (parent)
        {}

    virtual utils::device::deviceInfo::info_t getInfo () const = 0;

    virtual void setInfo (utils::device::deviceInfo::info_t&) = 0;
    };


class NetworkDeviceInfoWidget : public DeviceInfoWidget
    {
public:
    NetworkDeviceInfoWidget (QWidget* parent, utils::device::portNumber_t port) :
        DeviceInfoWidget (parent)
        {
        m_layout = new QFormLayout{ this };

        m_network       = new QComboBox{ this };
        m_addressV4     = new common::IpV4Field{ this };
        m_addressV6     = new common::IpV6Field{ this };
        m_port          = new QLineEdit{ this };

        m_network->addItem ("IPv4", QVariant::fromValue (QHostAddress::IPv4Protocol));
        m_network->addItem ("IPv6", QVariant::fromValue (QHostAddress::IPv6Protocol));

        m_layout->addRow ("Network Protocol",   m_network);
        m_layout->addRow ("Address",            m_addressV4);
        m_layout->addRow ("Address",            m_addressV6);
        m_layout->addRow ("Port",               m_port);

        m_port->setValidator (new QIntValidator{ 1, 65535 });

        if (control::ProtocolMetaClassBase::NO_DEFAULT_PORT != port)
            {
            m_port->setText (QString::number (port));
            }

        NetworkDeviceInfoWidget::networkProtoChanged (0);

        connect (m_network,
                &QComboBox::currentIndexChanged,
                 this,
                &NetworkDeviceInfoWidget::networkProtoChanged);

        setLayout (m_layout);
        }

    virtual void setInfo (utils::device::deviceInfo::info_t& info) override
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

    virtual utils::device::deviceInfo::info_t getInfo () const override
        {
        utils::device::socketInfo info;

        info.addr = m_activeIp->getValue ();
        info.port = m_port->text ().toInt ();

        return info;
        }

private:
    common::IpField*    m_activeIp = NULL;
    common::IpV4Field*  m_addressV4;
    common::IpV6Field*  m_addressV6;
    QLineEdit*          m_port;
    QComboBox*          m_network;
    QFormLayout*        m_layout;

    void networkProtoChanged (int idx)
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
        }
    };

class ComPortInfoWidget : public DeviceInfoWidget
    {
public:
    explicit ComPortInfoWidget (QWidget* parent) :
        DeviceInfoWidget (parent)
        {
        m_layout        = new QFormLayout{ this };

        m_comport       = new QComboBox{ this };
        m_baud          = new QComboBox{ this };

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

        m_layout->addRow ("Baud Rate", m_baud);
        m_layout->addRow ("COM Port", m_comport);
        }


    virtual void setInfo (utils::device::deviceInfo::info_t& info) override
        {
        const utils::device::serialInfo& com = std::get<utils::device::serialInfo> (info);

        common::setComboBoxIndexByText (*m_comport, com.port.portName ());
        common::setComboBoxIndexByUserData (*m_baud, com.baud);
        }

    virtual utils::device::deviceInfo::info_t getInfo () const override
        {
        utils::device::serialInfo info;

        info.baud = m_baud->currentData ().toInt ();
        info.port = m_comport->currentData ().value<QSerialPortInfo> ();

        return info;
        }
private:
    QComboBox*      m_comport;
    QComboBox*      m_baud;
    QFormLayout*    m_layout;
    };

Dialog::Dialog (QWidget* parent, control::ControllerBase* controller) :
    QDialog (parent)
    {
    auto controllers = control::getControllers ();

    QVBoxLayout*        layout  = new QVBoxLayout{ this };
    QDialogButtonBox*   buttons = new QDialogButtonBox{ QDialogButtonBox::Ok |
                                                        QDialogButtonBox::Cancel,
                                                        this };
    utils::device::portNumber_t port;

    if (NULL == controller)
        {
        port = controllers[0]->protocols[0]->defaultPort;
        }
    else
        {
        port = controller->getMetaClass ().protocols[0]->defaultPort;
        }

    m_layout = new QFormLayout;

    setWindowTitle ("Controller Settings");
    setWindowIcon (QIcon{ ":/icons/misc/gear.svg" });

    m_controller    = new common::OptionalDropdown{ this };
    m_protocol      = new common::OptionalDropdown{ this };
    m_transport     = new common::OptionalDropdown{ this };

    m_network       = new NetworkDeviceInfoWidget{ this, port };
    m_com           = new ComPortInfoWidget{ this } ;

    m_name          = new QLineEdit{ this };

    for (auto controller : controllers)
        {
        m_controller->addItem (controller->friendlyName.c_str (),
                               QVariant::fromValue (controller));
        }

    for (auto protocol : controllers[0]->protocols)
        {
        m_protocol->addItem (protocol->friendlyName.c_str (),
                             QVariant::fromValue (protocol));
        }

    for (auto transport : controllers[0]->protocols[0]->types)
        {
        m_transport->addItem (utils::device::typeNames[transport],
                              QVariant::fromValue (transport));
        }

    m_layout->addRow ("Name",               m_name);
    m_layout->addRow ("Controller Model",   m_controller);
    m_layout->addRow ("Protocol",           m_protocol);
    m_layout->addRow ("Transport Protocol", m_transport);

    if (NULL == controller)
        {
        setNetworkMode ();
        }
    else
        {
        utils::device::deviceInfo device = controller->getDeviceInfo ();

        m_controller->setIndexByUserData (&controller->getMetaClass ());
        m_protocol->setIndexByUserData (&controller->getProtocol ());
        m_transport->setIndexByUserData (device.type);

        if (utils::device::TYPE_SERIAL == device.type)
            {
            m_com->setInfo (device.info);
            setComMode ();
            }
        else
            {
            m_network->setInfo (device.info);
            setNetworkMode ();
            }
        }

    layout->addLayout (m_layout);
    layout->addWidget (m_com);
    layout->addWidget (m_network);
    layout->addWidget (buttons, 0, Qt::AlignHCenter);

    connect (buttons,
            &QDialogButtonBox::accepted,
             this,
            &QDialog::accept);

    connect (buttons,
            &QDialogButtonBox::rejected,
             this,
            &QDialog::reject);

    setLayout (layout);
    }

control::createControllerInfo Dialog::createController () const
    {
    utils::device::deviceInfo info;

    info.type = m_transport->currentData ().value<utils::device::type> ();
    info.info = m_active->getInfo ();

    return { m_controller->
                currentData ().
                    value<const control::ControllerMetaClassBase*> ()->
                            name,
            m_name->text ().toStdString (),
            m_protocol->
                currentData().
                    value<const control::ProtocolMetaClassBase*> ()->
                        name,
            info };
    }

void Dialog::setTransportProto (int idx)
    {
    if (utils::device::TYPE_SERIAL ==
        m_transport->itemData (idx).value<utils::device::type> ())
        {
        setComMode ();
        }
    else
        {
        setNetworkMode ();
        }
    }

void Dialog::setNetworkMode ()
    {
    m_network->setVisible (true);
    m_com->setVisible (false);
    m_active = m_network;
    }

void Dialog::setComMode ()
    {
    m_network->setVisible (false);
    m_com->setVisible (true);
    m_active = m_com;
    }


}