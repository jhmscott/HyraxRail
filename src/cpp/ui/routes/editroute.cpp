/**
 * @file        routes/editroute.cpp
 * @brief       Dialog box to edit/add routes
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/actuators/actuatorbutton.hpp>
#include <ui/common/autogrid.hpp>
#include <ui/common/toggleswitch.hpp>
#include <ui/routes/editroute.hpp>

#include <utils/string.hpp>

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QValidator>

namespace ui::routes
{

EditRouteDialog::EditRouteDialog (control::ControllerBase& controller, QWidget* parent) :
    common::FormDialog (parent)
    {
    QVBoxLayout*            layout      = new QVBoxLayout{ this };
    QHBoxLayout*            nameLayout  = new QHBoxLayout{ this };
    common::AutoGridLayout* btnLayout   = new common::AutoGridLayout{ common::AutoGridLayout::expand::ROW_FIRST,
                                                                      4,
                                                                      this };
    auto                    actuators   = controller.getActuators ();

    m_buttonList.reserve (actuators.size ());

    for (const auto& actuator : actuators)
        {
        btnLayout->addWidget (
            m_buttonList.emplace_back (
                new ActuatorIncludeButton{ actuator, this }));

        connect (m_buttonList.back (),
                &ActuatorIncludeButton::includeSwitched,
                 this,
                &FormDialog::inputChanged);
        }

    QLabel* label = new QLabel{ "Name:", this };

    nameLayout->addWidget (label);
    nameLayout->addWidget (m_name = new QLineEdit{ this });

    layout->addItem   (btnLayout);
    layout->addLayout (nameLayout);
    layout->addWidget (m_buttons, 0, Qt::AlignHCenter | Qt::AlignBottom);


    // Require non-empty string
    m_name->setValidator (
        new QRegularExpressionValidator{
                QRegularExpression{ utils::str::NON_EMPTY_REGEX }, this });

    setWindowTitle (QString::asprintf ("Add Route - %s",
                                       controller.getFriendlyName ().c_str ()));

    connect (m_name,
            &QLineEdit::textChanged,
             this,
            &FormDialog::inputChanged);

    inputChanged ();

    setLayout (layout);
    }

std::vector<control::routeMember> EditRouteDialog::getActuators () const
    {
    std::vector<ActuatorIncludeButton*> tmp;
    std::vector<control::routeMember>   members;

    tmp.reserve (m_buttonList.size ());

    std::copy_if (m_buttonList.begin (),
                  m_buttonList.end (),
                  std::back_inserter (tmp),
                  [] (ActuatorIncludeButton* btn) -> bool
                  { return btn->isIncluded (); });

    members.reserve (tmp.size ());

    std::transform (tmp.begin (),
                    tmp.end (),
                    std::back_inserter (members),
                    [] (ActuatorIncludeButton* btn) -> control::routeMember
                    { return { btn->getActuator (), btn->actuatorState () }; });

    return members;
    }

bool EditRouteDialog::hasAcceptableInput () const
    {
    // need to include at least one actuator
    return std::any_of (m_buttonList.begin (),
                        m_buttonList.end (),
                        [] (ActuatorIncludeButton* btn) -> bool
                        { return btn->isIncluded (); }) &&
           m_name->hasAcceptableInput ();
    }
}