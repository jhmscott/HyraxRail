/**
 * @file        config/dialog.cpp
 * @brief       Dialog box for the configuration of controllers
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/config/dialog.hpp>

#include <utils/string.hpp>

#include <QDialogButtonBox>
#include <QValidator>

namespace ui::config
{

Dialog::Dialog (QWidget* parent, control::ControllerBase* controller) :
    common::FormDialog (parent)
    {
    auto controllers = control::getControllers ();

    QVBoxLayout*        layout  = new QVBoxLayout{ this };
    utils::device::portNumber_t port;

    if (NULL == controller)
        {
        port = controllers[0]->protocols[0]->defaultPort;
        }
    else
        {
        port = controller->getMetaClass ().protocols[0]->defaultPort;
        }

    m_layout = new QFormLayout{ this };

    setWindowTitle ("Controller Settings");
    setWindowIcon (QIcon{ ":/icons/misc/gear.svg" });

    m_controller    = new common::OptionalDropdown{ this };
    m_protocol      = new common::OptionalDropdown{ this };
    m_transport     = new common::OptionalDropdown{ this };

    m_network       = new NetworkDeviceInfoWidget{ this, port };
    m_com           = new ComPortInfoWidget{ this } ;

    m_name          = new QLineEdit{ this };

    // Require non-empty string
    m_name->setValidator (
        new QRegularExpressionValidator{
                QRegularExpression{ utils::str::NON_EMPTY_REGEX }, this });

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
    layout->addWidget (m_buttons, 0, Qt::AlignHCenter);



    // Set the initial state of the OK button
    inputChanged ();

    connect (m_network,
            &DeviceInfoWidget::inputChanged,
             this,
            &Dialog::inputChanged);

    connect (m_com,
            &DeviceInfoWidget::inputChanged,
             this,
            &Dialog::inputChanged);

    connect (m_name,
            &QLineEdit::textChanged,
             this,
            &Dialog::inputChanged);

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

    inputChanged ();
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

bool Dialog::hasAcceptableInput() const
    {
    return m_active->hasAcceptableInput () &&
           m_name->hasAcceptableInput ();
    }

}