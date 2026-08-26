
#include <ui/common/utils.hpp>
#include <ui/trains/editloco.hpp>

#include <utils/string.hpp>

#include <QBoxLayout>

namespace ui::trains
{
EditLocoDialog::EditLocoDialog (const control::ControllerManager&   manager,
                                QWidget*                            parent,
                                const control::ControllerBase*      controller,
                                const layout::Locomotive*           loco) :
    common::FormDialog (parent),
    m_manager (manager),
    m_edit (NULL != loco)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    m_form = new QFormLayout;

    if (NULL == loco) // Add mode
        {
        m_form->addRow (new QLabel{ this },
                        m_controller = m_controllerCb = new QComboBox{ this });

        for (const control::ControllerBase& controllerToAdd : m_manager)
            {
            auto ud = QVariant::fromValue<const control::ControllerBase*> (&controllerToAdd);
            m_controllerCb->addItem (controllerToAdd.getFriendlyName ().c_str (), ud);
            }

        if (NULL != controller)
            {
            common::setComboBoxIndexByUserData (*m_controllerCb, controller);
            }
        else
            {
            controller = &m_manager[0];
            }

        m_controllerCb->setObjectName ("EditLocoController");

        connect (m_controllerCb,
                &QComboBox::currentIndexChanged,
                 this,
                &EditLocoDialog::onControllerChange);
        }
    else // edit mode
        {
        m_controllerCb = NULL;
        m_form->addRow (new QLabel{ this },
                        m_controller = new QLabel{ controller->getFriendlyName ().c_str (), this });

        m_controller->setObjectName ("EditLocoControllerLabel");
        }

    m_form->addRow (new QLabel{ this }, m_name      = new QLineEdit{ this });
    m_form->addRow (new QLabel{ this }, m_protocol  = new common::SchemeComboBox{ this });
    m_form->addRow (new QLabel{ this }, m_address   = new QSpinBox{ this });

    m_name->setObjectName     ("EditLocoName");
    m_protocol->setObjectName ("EditLocoProtocol");
    m_address->setObjectName  ("EditLocoAddress");

    populateProtocols (*controller);

    setWindowIcon ("misc/train");

    layout->addLayout (m_form);
    layout->addWidget (m_buttons, 0, Qt::AlignHCenter | Qt::AlignBottom);

    if (NULL != loco)
        {
        m_name->setText     (loco->getName ().c_str ());
        m_address->setValue (loco->getAddress ());

        common::setComboBoxIndexByUserData (*m_protocol, loco->getProtocol ());
        }

    limitAddress ();

    m_name->setValidator (new QRegularExpressionValidator{
                                    QRegularExpression{ utils::str::NON_EMPTY_REGEX } });

    connect (m_name,
            &QLineEdit::textChanged,
             this,
            &common::FormDialog::inputChanged);

    connect (m_address,
            &QSpinBox::valueChanged,
             this,
            &common::FormDialog::inputChanged);

    connect (m_protocol,
            &QComboBox::currentIndexChanged,
             this,
            &EditLocoDialog::onProtocolChange);

    setLabels ();

    setLayout (layout);

    emit inputChanged ();
    }

bool EditLocoDialog::hasAcceptableInput () const
    {
    return m_name->hasAcceptableInput () &&
           m_address->hasAcceptableInput ();
    }

void EditLocoDialog::setLabels ()
    {
    setWindowTitle (m_edit ? tr ("Edit Locomotive") : tr ("Add Locomotive"));

    common::setFormRowText (*m_form, *m_controller, tr ("Controller"));
    common::setFormRowText (*m_form, *m_name,       tr ("Name"));
    common::setFormRowText (*m_form, *m_protocol,   tr ("Protocol"));
    common::setFormRowText (*m_form, *m_address,    tr ("Address"));
    }

void EditLocoDialog::populateProtocols (const control::ControllerBase& controller)
    {
    const auto& metaClass = controller.getLocoMetaClass ();

    for (int ii = 0; ii < layout::TRACK_PROTO_UNKNOWN; ++ii)
        {
        if (metaClass.protocols[ii])
            {
            m_protocol->addItem (layout::PROTOCOLS[ii].icon,
                                 layout::PROTOCOLS[ii].description,
                                 QVariant::fromValue (
                                     static_cast<layout::trackProtocol> (ii)));
            }
        }
    }

void EditLocoDialog::limitAddress ()
    {
    layout::trackProtocol protocol = m_protocol->currentData ().value<layout::trackProtocol> ();

    m_address->setMinimum (1);
    m_address->setMaximum (layout::PROTOCOLS[protocol].numAddresses);
    }


void EditLocoDialog::onControllerChange (int idx)
    {
    const control::ControllerBase*  controller  = m_controllerCb->itemData (idx).
                                                    value<const control::ControllerBase*> ();

    layout::trackProtocol           current     = m_protocol->currentData ().
                                                    value<layout::trackProtocol> ();
    m_protocol->clear ();

    populateProtocols (*controller);

    // Restore the previous selection if it exists for the new set
    common::setComboBoxIndexByUserData (*m_protocol, current);
    limitAddress ();

    emit inputChanged ();
    }

void EditLocoDialog::onProtocolChange (int idx)
    {
    limitAddress ();

    emit inputChanged ();
    }

} // namespace ui::trains