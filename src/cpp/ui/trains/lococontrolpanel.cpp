/**
 * @file        trains/lococontrolpanel.cpp
 * @brief       Control panel for a single locomotive
 * @author      Justin Scott
 * @date        2026-03-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/seperator.hpp>
#include <ui/common/utils.hpp>

#include <ui/trains/lococontrolpanel.hpp>


namespace ui::trains
{

struct protocolInfo
    {
    const char* iconPath;
    const char* tooltip;
    };

static const protocolInfo PROTOCOLS[] =
    {
    { ":/icons/trains/dcc.svg",     "NMRA DCC (Digital Command Control)"    },
    { ":/icons/trains/mfx.svg",     "MFX (Märklin Digital)"                 },
    { ":/icons/trains/mm.svg",      "Märklin-Motorola"                      },
    { ":/icons/misc/question.svg",  ""                                      }
    };

LocoControlPanel::LocoControlPanel (control::ControllerManager* controllers, vAlignment align, QWidget* parent) :
    QWidget (parent)
    {

    QVBoxLayout* mainLayout     = new QVBoxLayout{ this };
    QHBoxLayout* controlLayout  = new QHBoxLayout{ this };
    QHBoxLayout* trainSelLayout = new QHBoxLayout{ this };

    m_controllerInfo = new ControllerInfo (controllers->size () > 0 ? &((*controllers)[0]) : NULL, this, false);

    m_locos = new QComboBox{ this };

    m_locos->setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);

    for (control::ControllerBase& controller : *controllers)
        {
        add (controller);
        }

    protocolInfo proto = PROTOCOLS[layout::TRACK_PROTO_UNKNOWN];

    if (controllers->size () > 0)
        {
        m_locos->setCurrentIndex (0);

        m_currentLoco = *m_locos->currentData ().value<layout::Locomotive*> ();
        m_currentLoco.requestControl ();

        proto = PROTOCOLS[m_currentLoco.getProtocol ()];
        }

    m_proto = new QPushButton{ QIcon{ proto.iconPath }, "", this };

    m_proto->setToolTip (proto.tooltip);
    m_proto->setIconSize (QSize{ 40, 30 });
    m_proto->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    common::makeFrameless (*m_proto);

    connect (m_locos,
            &QComboBox::currentIndexChanged,
             this,
            &LocoControlPanel::onLocoChange);

    connect (controllers,
            &control::ControllerManager::controllerAdded,
             this,
            &LocoControlPanel::add);

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
        }
    else // (vAlignment::RIGHT == align)
        {
        controlLayout->addWidget (m_functions);
        controlLayout->addWidget (m_speed);

        trainSelLayout->addWidget (m_locos);
        trainSelLayout->addWidget (m_proto);
        }

    mainLayout->addWidget (m_controllerInfo);
    mainLayout->addLayout (trainSelLayout);
    mainLayout->addLayout (controlLayout);

    setLayout (mainLayout);
    }

void LocoControlPanel::add (control::ControllerBase& controller)
    {
    for (layout::Locomotive& loco : controller.getLocomotives ())
        {
        std::string         name        = loco.getName ();
        layout::Locomotive* locoInCb    = new layout::Locomotive{ std::move (loco) };

        m_locos->addItem (QIcon{ ":/icons/misc/train.svg" },
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

    layout::Locomotive* newLoco = m_locos->currentData ().value<layout::Locomotive*> ();
    protocolInfo        proto   = PROTOCOLS[layout::TRACK_PROTO_UNKNOWN];

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

        proto = PROTOCOLS[m_currentLoco.getProtocol ()];

        m_controllerInfo->setController (
            *static_cast<control::ControllerBase*> (
                m_currentLoco.getController ()));
        }

    m_proto->setIcon (QIcon{ proto.iconPath });
    m_proto->setToolTip (proto.tooltip);
    }


void LocoControlPanel::locoDeleted ()
    {
    layout::Locomotive* loco = static_cast<layout::Locomotive*> (sender ());

    common::removeComboBoxItemByUserData (*m_locos, loco);
    }
}
