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
ControllerInfo::ControllerInfo (control::ControllerManager& manager,
                                control::ControllerBase*    controller,
                                QWidget*                    parent,
                                bool                        includeDelete) :
    QWidget (parent),
    m_manager (manager),
    m_controller (controller)
    {
    QHBoxLayout* layout = new QHBoxLayout{ this };

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

    m_stop->setIconSize (utils::resources::ICON_SIZE_NORMAL);
    m_stop->setCheckable (true);
    // m_stop->setTooltips ("Go", "Emergency Stop");

    m_connectionIcon->setIconSize (utils::resources::ICON_SIZE_NORMAL);
    ui::common::makeFrameless (*m_connectionIcon);

    m_settings->setIconSize (utils::resources::ICON_SIZE_NORMAL);
    common::makeFrameless (*m_settings);

    m_nameLabel = new QLabel{ NULL == controller ? "-" : controller->getFriendlyName ().c_str (), this};

    QFont   font = QApplication::font ();

    font.setPixelSize (utils::resources::FONT_SIZE_LARGE);

    m_nameLabel->setFont (font);


    layout->addWidget (m_connectionIcon,    0, Qt::AlignLeft);
    layout->addWidget (m_nameLabel,         0, Qt::AlignCenter);
    layout->addWidget (m_stop,              0, Qt::AlignRight);
    layout->addWidget (m_settings,          0, Qt::AlignRight);

    if (includeDelete)
        {
        m_deleteBtn = new common::PointedIconButton{ "misc/trash", this };
        m_deleteBtn->setIconSize (utils::resources::ICON_SIZE_NORMAL);

        common::makeFrameless (*m_deleteBtn);

        layout->addWidget (m_deleteBtn, 0, Qt::AlignRight);

        connect (m_deleteBtn,
                &QPushButton::released,
                 this,
                &ControllerInfo::onDelete);

        connect (&m_manager,
                 &control::ControllerManager::refreshStarted,
                  this,
                 &ControllerInfo::refreshStarted);

        connect (&m_manager,
                 &control::ControllerManager::cacheRebuilt,
                  this,
                 &ControllerInfo::refreshComplete);
        }

    if (NULL == controller)
        {
        clear ();
        }
    else
        {
        setController (*controller);
        }

    connect (m_stop,
            &QPushButton::toggled,
             this,
            &ControllerInfo::eStop);

    connect (m_settings,
            &QPushButton::released,
             this,
            &ControllerInfo::onConfig);

    connect (&m_manager,
             &control::ControllerManager::connectionHealthChanged,
              this,
             &ControllerInfo::refreshHealthIcon);

    setLayout (layout);
    }

void ControllerInfo::clear ()
    {
    m_controller = NULL;

    m_stop->setDisabled (true);
    m_settings->setDisabled (true);
    m_nameLabel->setText ("-");

    setToolTip ("");

    setNoControllerConfigured ();
    }

void ControllerInfo::setController (control::ControllerBase& controller)
    {
    m_controller = &controller;

    m_stop->setDisabled (false);
    m_settings->setDisabled (false);
    m_nameLabel->setText (controller.getFriendlyName ().c_str ());

    m_stopper = controller.getCapabilityController<layout::EmergencyStopController> ();

    if (NULL == m_stopper ||
        not m_controller->getConnectionHealth ().isConnected ())
        {
        m_stop->setDisabled (true);
        }
    else
        {
        m_stop->setEnabled (true);
        m_stop->setChecked (m_stopper->isEStopped ());
        }

    setToolTip (tr ("Model : %1").arg (controller.getMetaClass ().friendlyName.c_str ()));

    setHealth (m_controller->getConnectionHealth ());
    }

void ControllerInfo::refreshHealthIcon (const control::ControllerBase& controller)
    {
    if (&controller == m_controller)
        {
        setHealth (m_controller->getConnectionHealth ());
        }
    }

void ControllerInfo::setNoControllerConfigured ()
    {
    setHealth (control::ConnectionWorkerThread::health
              {
                  control::ConnectionWorkerThread::HEALTH_UNAVAILABLE,
                  std::chrono::milliseconds { 0 }
              });
    }

void ControllerInfo::refreshStarted (const control::ControllerBase &controller)
    {
    if (&controller == m_controller)
        {
        m_deleteBtn->setDisabled (true);
        }
    }

void ControllerInfo::refreshComplete(const control::ControllerBase &controller)
    {
    if (&controller == m_controller)
        {
        m_deleteBtn->setDisabled (false);
        }
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
        m_connectionIcon->setToolTip (tr ("Disconnected"));
        }
    else if (control::ConnectionWorkerThread::HEALTH_UNAVAILABLE == health.level)
        {
        m_connectionIcon->setToolTip ("N/A");
        }
    else if (control::ConnectionWorkerThread::HEALTH_CONNECTED == health.level)
        {
        m_connectionIcon->setToolTip (tr ("Connected"));
        }
    else
        {
        m_connectionIcon->setToolTip (tr ("Ping %1 ms").arg (health.ping.count ()));
        }

    // Disable stop if connection cannot be established
    m_stop->setEnabled (health.isConnected ());
    }

void ControllerInfo::onConfig ()
    {
    config::Dialog dlg{ this, m_controller };

    if (QDialog::Accepted == dlg.exec ())
        {
        auto info = dlg.createController ();

        if (m_controller->getDeviceInfo () != info.device)
            {
            m_controller->setDeviceInfo (info.device);
            }    
        
        if (m_controller->getFriendlyName () != info.friendlyName)
            {
            m_controller->setFriendlyName (info.friendlyName);
            }
        }
    }

void ControllerInfo::onDelete ()
    {
    QString msg =   tr ("Are you sure you want to delete %1 ?").arg (
                                    m_controller->getFriendlyName ().c_str ());

    if (QMessageBox::Yes == QMessageBox::question (this,
                                                   tr ("Delete Controller"),
                                                   msg))
        {
        m_manager.remove (*m_controller);
        }
    }
}
