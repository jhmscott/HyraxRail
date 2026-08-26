/**
 * @file        trains/lococontrolpanel.cpp
 * @brief       Control panel for a single locomotive
 * @author      Justin Scott
 * @date        2026-03-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/addbtn.hpp>
#include <ui/common/seperator.hpp>
#include <ui/common/utils.hpp>

#include <ui/trains/editloco.hpp>
#include <ui/trains/lococontrolpanel.hpp>

#include <QMessageBox>

namespace ui::trains
{


LocoControlPanel::LocoControlPanel (control::ControllerManager& controllers,
                                    vAlignment                  align,
                                    QWidget*                    parent) :
    QWidget (parent),
    m_controllers (controllers)
    {
    QVBoxLayout* mainLayout     = new QVBoxLayout{ this };
    QHBoxLayout* controlLayout  = new QHBoxLayout;
    QHBoxLayout* trainSelLayout = new QHBoxLayout;

    m_controllerInfo = new ControllerInfo{ controllers.size () > 0 ?
                                          &(controllers[0]) : NULL,
                                           this,
                                           false };

    m_locos = new common::SchemeComboBox{ this };

    m_locos->setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);

    for (control::ControllerBase& controller : controllers)
        {
        addController (controller);
        }

    layout::protocolInfo proto = layout::PROTOCOLS[layout::TRACK_PROTO_UNKNOWN];

    if (m_locos->count () > 0)
        {
        m_locos->setCurrentIndex (0);

        m_currentLoco = *m_locos->currentData ().value<layout::Locomotive*> ();
        m_currentLoco.requestControl ();

        proto = layout::PROTOCOLS[m_currentLoco.getProtocol ()];
        }

    m_proto = new common::SchemeIconButton{ proto.icon, this };

    m_proto->setToolTip (proto.description);
    m_proto->setIconSize (QSize{ 40, 30 });
    m_proto->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    common::makeFrameless (*m_proto);

    common::AddButton* add = new common::AddButton{ this };

    add->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    common::PointedIconButton* edit = new common::PointedIconButton{ "misc/pencil", this };

    edit->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);
    edit->setIconSize (QSize{ 20, 20 });
    common::makeFrameless (*edit);


    common::PointedIconButton* deleteBtn = new common::PointedIconButton{ "misc/trash", this };

    deleteBtn->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);
    deleteBtn->setIconSize (QSize{ 20, 20 });
    common::makeFrameless (*deleteBtn);

    connect (m_locos,
            &QComboBox::currentIndexChanged,
             this,
            &LocoControlPanel::onLocoChange);

    connect (&controllers,
             &control::ControllerManager::controllerAdded,
              this,
             &LocoControlPanel::addController);

    connect (add,
            &common::AddButton::addPressed,
             this,
            &LocoControlPanel::addLoco);

    connect (edit,
            &common::PointedIconButton::released,
             this,
            &LocoControlPanel::editLoco);

    connect (deleteBtn,
            &common::PointedIconButton::released,
             this,
            &LocoControlPanel::deleteLoco);

    m_speed = new SpeedControlWidget{ this };
    m_speed->setLocomotive (m_currentLoco);

    m_functions = new FunctionPanel{ align, this };
    m_functions->setLocomotive (m_currentLoco);

    trainSelLayout->setContentsMargins (0, 0, 0, 0);

    if (vAlignment::LEFT == align)
        {
        controlLayout->addWidget (m_speed);
        controlLayout->addWidget (m_functions);

        trainSelLayout->addWidget (m_proto);
        trainSelLayout->addWidget (m_locos);
        trainSelLayout->addWidget (edit);
        trainSelLayout->addWidget (deleteBtn);
        trainSelLayout->addWidget (add);
        }
    else // (vAlignment::RIGHT == align)
        {
        controlLayout->addWidget (m_functions);
        controlLayout->addWidget (m_speed);

        trainSelLayout->addWidget (add);
        trainSelLayout->addWidget (deleteBtn);
        trainSelLayout->addWidget (edit);
        trainSelLayout->addWidget (m_locos);
        trainSelLayout->addWidget (m_proto);
        }

    mainLayout->addWidget (m_controllerInfo);
    mainLayout->addLayout (trainSelLayout);
    mainLayout->addLayout (controlLayout);

    setLayout (mainLayout);
    }

void LocoControlPanel::addController (control::ControllerBase& controller)
    {
    for (layout::Locomotive& loco : controller.getLocomotives ())
        {
        std::string         name        = loco.getName ();
        layout::Locomotive* locoInCb    = new layout::Locomotive{ std::move (loco) };

        m_locos->addItem ("misc/train",
                          name.c_str (),
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

    layout::Locomotive*     newLoco = m_locos->currentData ().value<layout::Locomotive*> ();
    layout::protocolInfo    proto   = layout::PROTOCOLS[layout::TRACK_PROTO_UNKNOWN];

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

        proto = layout::PROTOCOLS[m_currentLoco.getProtocol ()];

        m_controllerInfo->setController (
            *static_cast<control::ControllerBase*> (
                m_currentLoco.getController ()));
        }

    m_proto->setIcon (proto.icon);
    m_proto->setToolTip (proto.description);
    }


void LocoControlPanel::locoDeleted ()
    {
    layout::Locomotive* loco = static_cast<layout::Locomotive*> (sender ());

    common::removeComboBoxItemByUserData (*m_locos, loco);
    }

void LocoControlPanel::addLoco ()
    {
    EditLocoDialog dlg
        {
        m_controllers,
        this,
        static_cast<control::ControllerBase*> (m_currentLoco.getController ())
        };

    dlg.exec ();
    }

void LocoControlPanel::editLoco ()
    {
    EditLocoDialog dlg
        {
        m_controllers,
        this,
        m_currentLoco
        };

    if (QDialog::Accepted == dlg.exec ())
        {
        if (m_currentLoco.getName () != dlg.getName ())
            {
            m_currentLoco.setName (dlg.getName ());
            }

        if (m_currentLoco.getAddress () != dlg.getAddress ())
            {
            m_currentLoco.setAddress (dlg.getAddress ());
            }

        if (m_currentLoco.getProtocol () != dlg.getProtocol ())
            {
            m_currentLoco.setProtocol (dlg.getProtocol ());
            }
        }
    }

void LocoControlPanel::deleteLoco ()
    {
    QString msg = tr ("Would you like to delete locomotive \"%1\"?").
                                            arg (m_currentLoco.getName ());

    if (QMessageBox::Yes == QMessageBox::question (this,
                                                   tr ("Delete Locomotive"),
                                                   msg))
        {

        }
    }
}
