
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
    common::FormDialog (parent),
    m_edit (NULL != actuator)
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

    QVBoxLayout* layout = new QVBoxLayout{ this };

    m_form = new QFormLayout;

    m_form->addRow (new QLabel{ this }, m_name      = new QLineEdit{ this });
    m_form->addRow (new QLabel{ this }, m_address   = new QSpinBox{ this });
    m_form->addRow (new QLabel{ this }, m_icon      = new common::SchemeComboBox{ this });
    m_form->addRow (new QLabel{ this }, m_mode      = new QComboBox{ this });
    m_form->addRow (new QLabel{ this }, m_duration  = new QComboBox{ this });

    common::refreshStyleSheetOnColorSchemeChange (*m_mode);
    common::refreshStyleSheetOnColorSchemeChange (*m_duration);

    setLabels ();

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

    layout->addLayout (m_form);
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

void EditActuatorDialog::updateTexts ()
    {
    for (int ii = 0; ii < m_icon->count (); ++ii)
        {
        const auto& icon = m_icon->itemData (ii).value<layout::actuatorIcon> ();

        m_icon->setItemText (ii, resources::getIconInfo (icon).title);
        }

    m_mode->setItemText (0, tr ("Switch"));
    m_mode->setItemText (1, tr ("Pulse"));

    setLabels ();
    }

void EditActuatorDialog::setLabels ()
    {
    setWindowTitle (m_edit ? tr ("Edit Switching Item") : tr ("Add Switching Item"));

    common::setFormRowText (*m_form, *m_name,       tr ("Name"));
    common::setFormRowText (*m_form, *m_address,    tr ("Address"));
    common::setFormRowText (*m_form, *m_icon,       tr ("Icon"));
    common::setFormRowText (*m_form, *m_mode,       tr ("Mode"));
    common::setFormRowText (*m_form, *m_duration,   tr ("Duration"));
    }

bool EditActuatorDialog::hasAcceptableInput () const
    {
    return m_name->hasAcceptableInput ();
    }

} // namespace ui::actuators
