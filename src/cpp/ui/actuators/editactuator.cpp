
#include <ui/actuators/editactuator.hpp>
#include <ui/actuators/resources.hpp>
#include <ui/common/utils.hpp>

#include <utils/string.hpp>

#include <QFormLayout>
#include <QValidator>

namespace ui::actuators
{

EditActuatorDialog::EditActuatorDialog (control::ControllerBase&    controller,
                                        QWidget*                    parent,
                                        const layout::Actuator*     actuator) :
    common::FormDialog (parent)
    {
    static const int durations[] =
        {
        50,
        100,
        150,
        200,
        250,
        500,
        750
        };

    QFormLayout* form   = new QFormLayout;
    QVBoxLayout* layout = new QVBoxLayout{ this };

    form->addRow ("Name",     m_name      = new QLineEdit{ this });
    form->addRow ("Address",  m_address   = new QSpinBox{ this });
    form->addRow ("Icon",     m_icon      = new QComboBox{ this });
    form->addRow ("Mode",     m_mode      = new QComboBox{ this });
    form->addRow ("Duration", m_duration  = new QComboBox{ this });

    m_name->setValidator (new QRegularExpressionValidator
                                {
                                QRegularExpression{ utils::str::NON_EMPTY_REGEX },
                                this
                                });

    // TODO: this should be determined by track protocol
    m_address->setMinimum (1);
    m_address->setMaximum (80);
    m_address->setButtonSymbols (QAbstractSpinBox::PlusMinus);

    m_mode->addItem ("Switch", QVariant::fromValue (layout::actuatorMode::SWITCH));
    m_mode->addItem ("Pulse",  QVariant::fromValue (layout::actuatorMode::PULSE));

    for (uint ii = 0; ii < layout::NUM_TRUE_ICONS; ++ii)
        {
        layout::actuatorIcon    icon = static_cast<layout::actuatorIcon> (ii);
        auto                    info = resources::getIconInfo (icon);

        m_icon->addItem (info.off,
                         info.title,
                         QVariant::fromValue (icon));
        }

    for (uint ii = 0; ii < std::size (durations); ++ii)
        {
        m_duration->addItem (QString::asprintf ("%d ms", durations[ii]),
                             QVariant::fromValue (durations[ii]));
        }

    layout->addLayout (form);
    layout->addWidget (m_buttons, 0, Qt::AlignHCenter | Qt::AlignBottom);

    if (NULL != actuator)
        {
        m_name->setText     (actuator->getName ().c_str ());
        m_address->setValue (actuator->getAddress ());

        common::setComboBoxIndexByUserData (*m_icon,        actuator->getIcon ());
        common::setComboBoxIndexByUserData (*m_mode,        actuator->getMode ());
        common::setComboBoxIndexByUserData (*m_duration,    actuator->getDuration ());
        }

    connect (m_name,
            &QLineEdit::textChanged,
             this,
            &FormDialog::inputChanged);

    inputChanged ();

    setLayout (layout);
    }

bool EditActuatorDialog::hasAcceptableInput () const
    {
    return m_name->hasAcceptableInput ();
    }

} // namespace ui::actuators
