/**
 * @file        actuators/actuatorbutton.cpp
 * @brief       Layout switching device toggle button
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/actuators/actuatorbutton.hpp>
#include <ui/actuators/editactuator.hpp>
#include <ui/actuators/resources.hpp>

#include <QBoxLayout>
#include <QMessageBox>

namespace ui::actuators
{
ActuatorButton::ActuatorButton (const layout::Actuator&     actuator,
                                QWidget*                    parent,
                                bool                        dummy,
                                control::AutomationManager* automations) :
    common::HoldToOpenMenu<QWidget> (parent),
    m_actuator (actuator),
    m_dummy (dummy),
    m_automations (automations)
    {
    auto icon = resources::getIconInfo (actuator.getIcon ());

    QVBoxLayout* layout = new QVBoxLayout{ this };

    m_button = new common::IconToggle
        {
        icon.icon,
        false,
        parent
        };

    m_button->setChecked (actuator.get ());

    m_name = new QLabel{ actuator.getName ().c_str (), this };

    m_button->setFixedSize (50, 50);
    m_button->setIconSize (QSize{ 30, 30 });

    m_name->setAlignment (Qt::AlignTop);

    layout->addWidget (m_button);
    layout->addWidget (m_name, 0, Qt::AlignHCenter);

    setContentsMargins (0, 20, 0, 0);
    setContextMenuPolicy (Qt::ActionsContextMenu);

    createMenu ();

    connect (m_button,
            &QPushButton::toggled,
             this,
            &ActuatorButton::onToggle);

    // If it's a dummy, this would be annoying to override the user's selection constantly
    if (not dummy)
        {
        connect (&m_actuator,
                 &layout::Actuator::stateChanged,
                  m_button,
                 &QPushButton::setChecked);
        }

    setLayout (layout);

    if (not dummy)
        {
        m_actuator.request ();
        }
    }

ActuatorButton::~ActuatorButton ()
    {
    if (not m_dummy)
        {
        m_actuator.release ();
        }
    }

void ActuatorButton::setState (bool state)
    {
    m_button->setChecked (state);

    if (not m_dummy)
        {
        m_actuator.set (state);
        }
    }

void ActuatorButton::createMenu ()
    {
    if (not m_dummy)
        {
        addAction (tr ("Delete"), this, &ActuatorButton::removeActuator);
        addAction (tr ("Edit"),   this, &ActuatorButton::editActuator);
        }
    }

void ActuatorButton::removeActuator ()
    {
    QString msg = tr ("Would you like to delete actuator \"%1\"?").
                                    arg (m_actuator.getName ().c_str ());

    if (NULL != m_automations)
        {
        std::vector<std::string> automations;

        for (control::AutomationTask& task : *m_automations)
            {
            control::AutomationItem& item = *task.getItem ();

            if (control::AutomationItem::type::ACTUATOR == item.getType () &&
                m_actuator == item.getActuator ())
                {
                automations.push_back (item.name ());
                }
            }

        if (automations.size () > 0)
            {
            msg += "\n\n";
            msg += tr ("The following automations will also be deleted:");
            msg += "\n";

            for (const std::string& name : automations)
                {
                msg += QString{ " ● %1" }.arg (name);
                }
            }
        }

    if (QMessageBox::Yes == QMessageBox::question (this,
                                                   tr ("Delete Actuator"),
                                                   msg))
        {
        m_actuator.remove ();
        emit actuatorDeleted ();
        }
    }

void ActuatorButton::editActuator ()
    {
    EditActuatorDialog dlg
        {
       *static_cast<control::ControllerBase*>
                            (m_actuator.getController ()),
        this,
       &m_actuator
        };

    if (QDialog::Accepted == dlg.exec ())
        {
        std::string             newName = dlg.getName ();
        layout::actuatorMode    newMode = dlg.getMode ();
        layout::actuatorIcon    newIcon = dlg.getIcon ();
        uint                    newAddr = dlg.getAddress ();
        uint                    newDur  = dlg.getDuration ();

        if (newName != m_actuator.getName ())
            {
            m_actuator.setName (newName);
            m_name->setText (newName.c_str ());
            }

        if (newMode != m_actuator.getMode ())
            {
            m_actuator.setMode (newMode);
            }

        if (newIcon != m_actuator.getIcon ())
            {
            auto icon = resources::getIconInfo (newIcon);

            m_actuator.setIcon (newIcon);
            m_button->setIcon (icon.icon);
            }

        if (newAddr != m_actuator.getAddress ())
            {
            m_actuator.setAddress (newAddr);
            }

        if (newDur != m_actuator.getDuration ())
            {
            m_actuator.setDuration (newDur);
            }
        }
    }

void ActuatorButton::onToggle (bool state)
    {
    if (not m_dummy)
        {
        m_actuator.set (state);
        }
    }

} // namespace ui::actuators