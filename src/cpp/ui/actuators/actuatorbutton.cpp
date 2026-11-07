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
    ComponentButton (automations, actuator, not dummy, parent),
    m_dummy (dummy)
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

    m_button->setFixedSize (utils::resources::PUSHBUTTON_SIZE);
    m_button->setIconSize (utils::resources::ICON_SIZE_LARGE);

    m_name->setAlignment (Qt::AlignTop);

    layout->addWidget (m_button);
    layout->addWidget (m_name, 0, Qt::AlignHCenter);

    setContentsMargins (0, 20, 0, 0);

    connect (m_button,
            &QPushButton::toggled,
             this,
            &ActuatorButton::onToggle);

    // If it's a dummy, this would be annoying to override the user's selection constantly
    if (not dummy)
        {
        connect (&m_component,
                 &layout::Actuator::stateChanged,
                  m_button,
                 &QPushButton::setChecked);
        }

    setLayout (layout);

    if (not dummy)
        {
        m_component.request ();
        }
    }

ActuatorButton::~ActuatorButton ()
    {
    if (not m_dummy)
        {
        m_component.release ();
        }
    }

void ActuatorButton::setState (bool state)
    {
    m_button->setChecked (state);

    if (not m_dummy)
        {
        m_component.set (state);
        }
    }


void ActuatorButton::editComponent ()
    {
    EditActuatorDialog dlg
        {
        this,
       &m_component
        };

    if (QDialog::Accepted == dlg.exec ())
        {
        std::string             newName = dlg.getName ();
        layout::actuatorMode    newMode = dlg.getMode ();
        layout::actuatorIcon    newIcon = dlg.getIcon ();
        uint                    newAddr = dlg.getAddress ();
        uint                    newDur  = dlg.getDuration ();

        if (newName != m_component.getName ())
            {
            m_component.setName (newName);
            m_name->setText (newName.c_str ());
            }

        if (newMode != m_component.getMode ())
            {
            m_component.setMode (newMode);
            }

        if (newIcon != m_component.getIcon ())
            {
            auto icon = resources::getIconInfo (newIcon);

            m_component.setIcon (newIcon);
            m_button->setIcon (icon.icon);
            }

        if (newAddr != m_component.getAddress ())
            {
            m_component.setAddress (newAddr);
            }

        if (newDur != m_component.getDuration ())
            {
            m_component.setDuration (newDur);
            }
        }
    }

void ActuatorButton::onToggle (bool state)
    {
    if (not m_dummy)
        {
        m_component.set (state);
        }
    }

} // namespace ui::actuators
