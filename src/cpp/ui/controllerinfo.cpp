/**
 * @file        controllerinfo.cpp
 * @brief       Widget to display the state of a controller
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>

#include <ui/common/pointedwidget.hpp>
#include <ui/controllerinfo.hpp>
#include <ui/config/dialog.hpp>


namespace ui
{

static constexpr int ICON_SIZE = 20;

ControllerInfo::ControllerInfo (control::ControllerBase* controller, QWidget* parent, bool includeDelete) :
    QWidget (parent),
    m_controller (controller)
    {
    QHBoxLayout* layout = new QHBoxLayout{ this };

    QPushButton* deleteBtn;

    m_settings = new common::PointedIconButton{ "misc/gear", this };

    m_connectionIcon = new common::SchemeIconButton{ "", this};

    QIcon icon;

    icon.addFile (":/icons/common/misc/circle-play.svg",
                  QSize{},
                  QIcon::Normal,
                  QIcon::On);
    icon.addFile (":/icons/common/misc/prohibit-inset.svg",
                  QSize{},
                  QIcon::Normal,
                  QIcon::Off);

    m_stop = new QPushButton{ icon, "", this };
    common::makeFrameless (*m_stop);

    m_stop->setIconSize (QSize{ ICON_SIZE, ICON_SIZE });
    m_stop->setCheckable (true);
    // m_stop->setTooltips ("Go", "Emergency Stop");

    m_connectionIcon->setIconSize (QSize{ ICON_SIZE, ICON_SIZE });
    ui::common::makeFrameless (*m_connectionIcon);

    m_settings->setIconSize (QSize{ ICON_SIZE, ICON_SIZE });
    common::makeFrameless (*m_settings);

    m_nameLabel = new QLabel{ NULL == controller ? "-" : controller->getFriendlyName ().c_str (), this};

    QFont   font = QApplication::font ();

    font.setPixelSize (16);

    m_nameLabel->setFont (font);


    layout->addWidget (m_connectionIcon,    0, Qt::AlignLeft);
    layout->addWidget (m_nameLabel,         0, Qt::AlignCenter);
    layout->addWidget (m_stop,              0, Qt::AlignRight);
    layout->addWidget (m_settings,          0, Qt::AlignRight);

    if (includeDelete)
        {
        deleteBtn = new common::PointedIconButton{ "misc/trash", this };
        deleteBtn->setIconSize (QSize{ ICON_SIZE, ICON_SIZE });

        common::makeFrameless (*deleteBtn);

        layout->addWidget (deleteBtn, 0, Qt::AlignRight);

        connect (deleteBtn,
                 &QPushButton::released,
                 this,
                 &ControllerInfo::onDelete);
        }

    if (NULL == controller)
        {
        clear ();
        }
    else
        {
        setController (*controller);
        }

    QTimer* timer = new QTimer{ this };

    timer->setInterval (1000);
    timer->start ();

    connect (timer,
             &QTimer::timeout,
             this,
             &ControllerInfo::refreshHealthIcon);

    connect (m_stop,
            &QPushButton::toggled,
             this,
            &ControllerInfo::eStop);

    connect (m_settings,
            &QPushButton::released,
             this,
            &ControllerInfo::onConfig);


    setLayout (layout);
    }

void ControllerInfo::clear ()
    {
    m_controller = NULL;

    m_stop->setDisabled (true);
    m_settings->setDisabled (true);
    m_nameLabel->setText ("-");

    setToolTip ("");

    refreshHealthIcon ();
    }

void ControllerInfo::setController (control::ControllerBase& controller)
    {
    m_controller = &controller;

    m_stop->setDisabled (false);
    m_settings->setDisabled (false);
    m_nameLabel->setText (controller.getFriendlyName ().c_str ());

    m_stop->setChecked (m_controller->isEStopped ());

    setToolTip (QString::asprintf ("Model: %s",
                                   controller.getMetaClass ().friendlyName.c_str ()));

    refreshHealthIcon ();
    }

void ControllerInfo::setHealth (control::ConnectionWorkerThread::health health)
    {
    static const utils::resources::Icon healthIcons[] =
        {
        "misc/cell-signal-x",
        "misc/cell-signal-low",
        "misc/cell-signal-medium",
        "misc/cell-signal-high",
        "misc/cell-signal-full",

        "misc/plugs-connected",
        "misc/unplug",

        "misc/cell-signal-grey"
        };
    ASSERT_ARRAY_LENGTH (healthIcons, control::ConnectionWorkerThread::NUM_HEALTH_VALUES);

    m_connectionIcon->setIcon (healthIcons[health.level]);

    if (control::ConnectionWorkerThread::HEALTH_DEAD            == health.level ||
        control::ConnectionWorkerThread::HEALTH_DISCONNECTED    == health.level)
        {
        m_connectionIcon->setToolTip ("Disconnected");
        }
    else if (control::ConnectionWorkerThread::HEALTH_UNAVAILABLE == health.level)
        {
        m_connectionIcon->setToolTip ("N/A");
        }
    else if (control::ConnectionWorkerThread::HEALTH_CONNECTED == health.level)
        {
        m_connectionIcon->setToolTip ("Connected");
        }
    else
        {
        m_connectionIcon->setToolTip (QString::asprintf ("Ping %d ms",
                                                         health.ping.count ()));
        }
    }

void ControllerInfo::onConfig ()
    {
    config::Dialog dialog{ this, m_controller };

    dialog.exec ();
    }


void ControllerInfo::onDelete ()
    {
    QString msg = QString::asprintf ("Are you sure you want to delete %s?",
                                     m_controller->getFriendlyName ().c_str ());

    if (QMessageBox::Yes == QMessageBox::question (this,
                                                   "Delete Controller",
                                                   msg))
        {
        emit deleted (*m_controller);
        }
    }
}