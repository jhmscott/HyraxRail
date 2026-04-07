/**
 * @file        trains/lococontrolpanel.cpp
 * @brief       Control panel for a single locomotive
 * @author      Justin Scott
 * @date        2026-03-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/utils.hpp>

#include <ui/trains/lococontrolpanel.hpp>


namespace ui::trains
{

LocoControlPanel::LocoControlPanel (const control::controllerList& controllers, vAlignment align, QWidget* parent) :
    QWidget (parent)
    {
    QVBoxLayout* mainLayout = new QVBoxLayout{ this };
    QHBoxLayout* controlLayout = new QHBoxLayout{ this };

    m_controllerInfo = new ControllerInfo (controllers.size () > 0 ? controllers[0] : NULL, this, false);

    m_locos = new QComboBox{ this };

    for (control::ControllerBase* controller : controllers)
        {
        add (*controller);
        }

    if (controllers.size () > 0)
        {
        m_locos->setCurrentIndex (0);

        m_currentLoco = *m_locos->currentData ().value<layout::Locomotive*> ();
        m_currentLoco.requestControl ();
        }

    connect (m_locos,
             &QComboBox::currentIndexChanged,
             this,
             &LocoControlPanel::onLocoChange);

    m_speed = new SpeedControlWidget{ this };
    m_speed->setLocomotive (m_currentLoco);

    m_functions = new FunctionPanel{ align, this };
    m_functions->setLocomotive (m_currentLoco);

    if (vAlignment::LEFT == align)
        {
        controlLayout->addWidget (m_speed);
        controlLayout->addWidget (m_functions);
        }
    else
        {
        controlLayout->addWidget (m_functions);
        controlLayout->addWidget (m_speed);
        }

    mainLayout->addWidget (m_controllerInfo);
    mainLayout->addWidget (m_locos);
    mainLayout->addLayout (controlLayout);

    setLayout (mainLayout);
    }

void LocoControlPanel::add (control::ControllerBase& controller)
    {
    for (layout::Locomotive& loco : controller.getLocomotives ())
        {
        std::string         name        = loco.getName ();
        layout::Locomotive* locoInCb    = new layout::Locomotive{ std::move (loco) };

        m_locos->addItem (name.c_str (),
                          QVariant::fromValue (locoInCb));

        connect (locoInCb,
                &layout::Locomotive::destroyed,
                 this,
                &LocoControlPanel::locoDeleted);
        }
    }


void LocoControlPanel::onLocoChange (int idx)
    {
    m_currentLoco.releaseControl ();

    layout::Locomotive* newLoco = m_locos->currentData ().value<layout::Locomotive*> ();

    if (NULL == newLoco)
        {
        m_currentLoco = {};

        m_controllerInfo->clear ();
        m_speed->clear ();
        m_functions->clear ();
        }
    else
        {
        m_currentLoco = *newLoco;
        m_currentLoco.requestControl ();

        m_speed->setLocomotive (m_currentLoco);
        m_functions->setLocomotive (m_currentLoco);

        m_controllerInfo->setController (
            *static_cast<control::ControllerBase*> (
                m_currentLoco.getController ()));
        }
    }


void LocoControlPanel::locoDeleted ()
    {
    layout::Locomotive* loco = static_cast<layout::Locomotive*> (sender ());

    common::removeComboBoxItemByUserData (*m_locos, loco);
    }
}
