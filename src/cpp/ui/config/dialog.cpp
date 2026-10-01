/**
 * @file        config/dialog.cpp
 * @brief       Dialog box for the configuration of controllers
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <names.hpp>

#include <ui/config/cominfo.hpp>
#include <ui/config/dialog.hpp>
#include <ui/config/networkinfo.hpp>

#include <utils/string.hpp>

#include <QDialogButtonBox>
#include <QValidator>

namespace ui::config
{

Dialog::Dialog (QWidget* parent, control::ControllerBase* controller) :
    common::FormDialog (parent),
    m_edit (NULL != controller)
    {
    auto controllers = control::getControllers ();
    const control::ControllerMetaClassBase* meta;

    QVBoxLayout*        layout  = new QVBoxLayout{ this };
    utils::device::portNumber_t port;

    if (NULL == controller)
        {
        meta = controllers[0];
        }
    else
        {
        meta = &controller->getMetaClass ();
        }

    port = meta->protocols[0]->defaultPort;

    m_layout = new QFormLayout{ this };

    setWindowIcon ("misc/gear");

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

    auto addControllerType =
        [this] (const control::ControllerMetaClassBase* meta)
        {
        m_controller->addItem (meta->friendlyName.c_str (),
                               QVariant::fromValue (meta));
        };

    if (NULL == controller)
        {
        for (auto controller : controllers)
            {
            addControllerType (controller);
            }
        }
    else
        {
        addControllerType (meta);
        }

    for (auto protocol : meta->protocols)
        {
        m_protocol->addItem (protocol->friendlyName.c_str (),
                             QVariant::fromValue (protocol));
        }

    const control::ProtocolMetaClassBase* protocol;

    if (NULL == controller)
        {
        protocol = meta->protocols[0];
        }
    else
        {
        protocol = &controller->getProtocol ();
        }

    for (auto transport : protocol->types)
        {
        m_transport->addItem (utils::device::typeNames[transport],
                              QVariant::fromValue (transport));
        }

    m_layout->addRow (new QLabel{ this }, m_name);
    m_layout->addRow (new QLabel{ this }, m_controller);
    m_layout->addRow (new QLabel{ this }, m_protocol);
    m_layout->addRow (new QLabel{ this }, m_transport);

    m_name      ->setObjectName (OBJNAME_CONFIG_DIALOG_NAME);
    m_controller->setObjectName (OBJNAME_CONFIG_DIALOG_CONTROLLER);
    m_protocol  ->setObjectName (OBJNAME_CONFIG_DIALOG_PROTOCOL);
    m_transport ->setObjectName (OBJNAME_CONFIG_DIALOG_TRANSPORT);

    if (NULL == controller)
        {
        setNetworkMode ();
        }
    else
        {
        utils::device::deviceInfo device = controller->getDeviceInfo ();

        m_name->setText (controller->getFriendlyName ().c_str ());
        m_controller->setIndexByUserData (meta);
        m_protocol->setIndexByUserData (protocol);
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

    setLabels ();

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

    connect (m_transport,
            &common::OptionalDropdown::currentIndexChanged,
             this,
            &Dialog::setTransportProto);

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

    setControllerTooltip ();
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

QString Dialog::getErrorString () const
    {
    QString error;

    if (not m_name->hasAcceptableInput ())
        {
        error = tr ("Enter a controller name");
        }
    else if (not m_active->hasAcceptableInput ())
        {
        error = m_active->getErrorString ();
        }

    return error;
    }

void Dialog::setLabels ()
    {
    common::setFormRowText (*m_layout, *m_name,         tr ("Name"));
    common::setFormRowText (*m_layout, *m_controller,   tr ("Controller Model"));
    common::setFormRowText (*m_layout, *m_protocol,     tr ("Protocol"));
    common::setFormRowText (*m_layout, *m_transport,    tr ("Transport Protocol"));

    if (m_edit)
        {
        setWindowTitle (tr ("Edit Controller Settings"));
        }
    else
        {
        setWindowTitle (tr ("Add Controller"));
        }

    setControllerTooltip ();
    }

void Dialog::setControllerTooltip ()
    {
    const QString CAPABILITY_NAMES[] =
        {
        tr ("Locomotive Controller"),
        tr ("Actuator Controller"),
        tr ("Route Controller"),
        tr ("Emergency Stop")
        };
    ASSERT_ARRAY_LENGTH (CAPABILITY_NAMES, control::NUM_CAPABILITIES);

    const auto& meta = *m_controller->currentData ().value<const control::ControllerMetaClassBase*> ();
    QString     tooltip;
    QString     hardware;
    QString     software;

    for (control::controllerCapability capability :
            utils::algorithm::EnumRange{ control::CAPABILITY_LOCOMOTIVE,
                                         control::NUM_CAPABILITIES })
        {
        if (meta.hardwareCapabilities[capability])
            {
            hardware += "  " + CAPABILITY_NAMES[capability] + "\n";
            }

        if (meta.softwareCapabilities[capability])
            {
            software += "  " + CAPABILITY_NAMES[capability] + "\n";
            }
        }

    tooltip += tr ("Hardware Capabilities:") + "\n";
    tooltip += hardware;

    if (not software.isEmpty ())
        {
        tooltip += "\n" + tr ("Software Capabilities:") + "\n";
        tooltip += software;
        }

    m_controller->setToolTip (tooltip);
    }

} // namespace ui::config