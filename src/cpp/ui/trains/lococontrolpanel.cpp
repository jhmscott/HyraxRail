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
                                    hAlignment                  align,
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
    m_proto->setIconSize (
#ifdef Q_OS_ANDROID
        QSize{ 60, 45 }
#else
        QSize{ 40, 30 }
#endif // Q_OS_ANDROID

    );
    m_proto->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    common::makeFrameless (*m_proto);

    m_add = new common::AddButton{ this };

    m_add->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    m_edit = new common::PointedIconButton{ "misc/pencil", this };

    m_edit->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_edit->setIconSize (utils::resources::ICON_SIZE_NORMAL);
    common::makeFrameless (*m_edit);


    m_delete = new common::PointedIconButton{ "misc/trash", this };

    m_delete->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_delete->setIconSize (utils::resources::ICON_SIZE_NORMAL);
    common::makeFrameless (*m_delete);

    connect (m_locos,
            &QComboBox::currentIndexChanged,
             this,
            &LocoControlPanel::onLocoChange);

    connect (&controllers,
             &control::ControllerManager::controllerAdded,
              this,
             &LocoControlPanel::addController);

    connect (m_add,
            &common::AddButton::addPressed,
             this,
            &LocoControlPanel::addLoco);

    connect (m_edit,
            &common::PointedIconButton::released,
             this,
            &LocoControlPanel::editLoco);

    connect (m_delete,
            &common::PointedIconButton::released,
             this,
            &LocoControlPanel::deleteLoco);

    m_speed = new SpeedControlWidget{ this };
    m_speed->setLocomotive (m_currentLoco);

    m_functions = new FunctionPanel{ align, this };
    m_functions->setLocomotive (m_currentLoco);

    trainSelLayout->setContentsMargins (0, 0, 0, 0);

    if (hAlignment::LEFT == align)
        {
        controlLayout->addWidget (m_speed);
        controlLayout->addWidget (m_functions);

        trainSelLayout->addWidget (m_proto);
        trainSelLayout->addWidget (m_locos);
        trainSelLayout->addWidget (m_edit);
        trainSelLayout->addWidget (m_delete);
        trainSelLayout->addWidget (m_add);
        }
    else // (vAlignment::RIGHT == align)
        {
        controlLayout->addWidget (m_functions);
        controlLayout->addWidget (m_speed);

        trainSelLayout->addWidget (m_add);
        trainSelLayout->addWidget (m_delete);
        trainSelLayout->addWidget (m_edit);
        trainSelLayout->addWidget (m_locos);
        trainSelLayout->addWidget (m_proto);
        }

    mainLayout->addWidget (m_controllerInfo);
    mainLayout->addLayout (trainSelLayout);
    mainLayout->addLayout (controlLayout);

    updateButtonStates ();

    setLayout (mainLayout);
    }

void LocoControlPanel::addLocoToCb (const layout::Locomotive& loco)
    {
    std::string         name        = loco.getName ();
    layout::Locomotive* locoInCb    = new layout::Locomotive{ loco };

    locoInCb->setParent (m_locos);

    m_locos->addItem ("misc/train",
                      name.c_str (),
                      QVariant::fromValue (locoInCb));

    connect (locoInCb,
            &layout::Locomotive::destroyed,
             this,
            &LocoControlPanel::locoDeleted);
    }

void LocoControlPanel::setProtocol (layout::trackProtocol proto)
    {
    layout::protocolInfo info = layout::PROTOCOLS[proto];

    m_proto->setIcon (info.icon);
    m_proto->setToolTip (info.description);
    }

void LocoControlPanel::updateButtonStates ()
    {
    bool hasLocos       = m_locos->count () > 0;
    bool hasController  = std::any_of (m_controllers.begin (),
                                       m_controllers.end (),
                                       [] (const control::ControllerBase& controller) -> bool
                                       { return controller.getConnectionHealth ().isConnected (); });

    m_delete->setEnabled (hasLocos && hasController);
    m_edit->setEnabled (hasLocos && hasController);
    m_add->setEnabled (hasController);
    }

void LocoControlPanel::addController (control::ControllerBase& controller)
    {
    for (const layout::Locomotive& loco : controller.getLocomotives ())
        {
        addLocoToCb (loco);
        }
    }


void LocoControlPanel::onLocoChange (int idx)
    {
    m_currentLoco.releaseControl ();

    layout::Locomotive*     newLoco = m_locos->currentData ().value<layout::Locomotive*> ();
    layout::trackProtocol   proto   = layout::TRACK_PROTO_UNKNOWN;

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

        proto = m_currentLoco.getProtocol ();

        m_controllerInfo->setController (
            *static_cast<control::ControllerBase*> (
                m_currentLoco.getController ()));
        }

    setProtocol (proto);
    }


void LocoControlPanel::locoDeleted ()
    {
    layout::Locomotive* loco = static_cast<layout::Locomotive*> (sender ());

    common::removeComboBoxItemByUserData (*m_locos, loco);

    updateButtonStates ();
    }

void LocoControlPanel::addLoco ()
    {
    EditLocoDialog dlg
        {
        m_controllers,
        this,
        static_cast<control::ControllerBase*> (m_currentLoco.getController ())
        };

    if (QDialog::Accepted == dlg.exec ())
        {
        control::ControllerBase* controller = const_cast<control::ControllerBase*>
                                                                    (dlg.getController ());

        layout::Locomotive loco = controller->createLocomotive (dlg.getName (),
                                                                dlg.getProtocol (),
                                                                dlg.getAddress (),
                                                                dlg.getFunctions ());

        addLocoToCb (loco);
        }
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
        std::string                     newName     = dlg.getName ();
        uint                            newAddress  = dlg.getAddress ();
        layout::trackProtocol           newProto    = dlg.getProtocol ();
        std::vector<layout::funcInfo>   newFunctions= dlg.getFunctions ();

        std::vector<layout::funcInfo>   oldFunctions= m_currentLoco.getFunctions ();

        if (m_currentLoco.getName () != newName)
            {
            int idx = m_locos->currentIndex (); // we only ever edit the current loco

            m_currentLoco.setName (newName);
            m_locos->setItemText (idx, newName.c_str ());
            }

        if (m_currentLoco.getAddress () != newAddress)
            {
            m_currentLoco.setAddress (newAddress);
            }

        if (m_currentLoco.getProtocol () != newProto)
            {
            m_currentLoco.setProtocol (newProto);
            setProtocol (newProto);
            }

        if (not std::equal (newFunctions.begin (),
                            newFunctions.end (),
                            oldFunctions.begin (),
                            oldFunctions.end ()))
            {
            m_currentLoco.setFunctions (newFunctions);
            // Forces an update of functions
            m_functions->setLocomotive (m_currentLoco);
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
        m_currentLoco.remove ();
        }
    }
}
