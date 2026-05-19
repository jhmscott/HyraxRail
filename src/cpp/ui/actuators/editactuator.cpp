
#include <ui/actuators/editactuator.hpp>
#include <ui/actuators/resources.hpp>
#include <ui/common/utils.hpp>

#include <utils/string.hpp>

#include <QApplication>
#include <QFormLayout>
#include <QStyleHints>
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

    form->addRow (tr ("Name"),     m_name      = new QLineEdit{ this });
    form->addRow (tr ("Address"),  m_address   = new QSpinBox{ this });
    form->addRow (tr ("Icon"),     m_icon      = new common::SchemeComboBox{ this });
    form->addRow (tr ("Mode"),     m_mode      = new QComboBox{ this });
    form->addRow (tr ("Duration"), m_duration  = new QComboBox{ this });

    setWindowIcon ("misc/split");

    m_name->setValidator (new QRegularExpressionValidator
                                {
                                QRegularExpression{ utils::str::NON_EMPTY_REGEX },
                                this
                                });

    // TODO: this should be determined by track protocol
    m_address->setMinimum (1);
    m_address->setMaximum (80);
    m_address->setButtonSymbols (QAbstractSpinBox::PlusMinus);

    m_mode->addItem (tr ("Switch"), QVariant::fromValue (layout::actuatorMode::SWITCH));
    m_mode->addItem (tr ("Pulse"),  QVariant::fromValue (layout::actuatorMode::PULSE));

    for (uint ii = 0; ii < layout::NUM_TRUE_ICONS; ++ii)
        {
        layout::actuatorIcon    icon = static_cast<layout::actuatorIcon> (ii);
        auto                    info = resources::getIconInfo (icon);

        m_icon->addItem (info.icon,
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
