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
ActuatorButton::ActuatorButton (const layout::Actuator& actuator, QWidget* parent, bool dummy) :
    QWidget (parent),
    m_actuator (actuator),
    m_dummy (dummy)
    {
    auto icon = resources::getIconInfo (actuator.getIcon ());

    QVBoxLayout* layout = new QVBoxLayout{ this };

    m_button = new common::IconToggle
        {
        icon.on,
        icon.off,
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

    if (not dummy)
        {
        addAction ("delete",this, &ActuatorButton::removeActuator);
        addAction ("edit",  this, &ActuatorButton::editActuator);
        }

    connect (m_button,
            &QPushButton::toggled,
             this,
            &ActuatorButton::onToggle);

    setLayout (layout);

    if (not dummy)
        {
        m_actuator.request ();
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

void ActuatorButton::removeActuator ()
    {
    if (QMessageBox::Yes == QMessageBox::question (this,
                                                   "Delete Actuator",
                                                   QString::asprintf ("Would you like to delete %s?",
                                                                      m_actuator.getName ().c_str ())))
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
            m_button->setIcons (icon.on, icon.off);
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
}