/**
 * @file        mainwidget.cpp
 * @brief       Top level widget
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/mainwidget.hpp>
#include <ui/actuators/actuatorpanel.hpp>
#include <ui/config/configpanel.hpp>
#include <ui/sensors/sensorpanel.hpp>
#include <ui/trains/dualcontrol.hpp>
#include <ui/routes/routepanel.hpp>

#include <utils/algorithm.hpp>
#include <utils/os.hpp>

namespace ui
{
MainWidget::MainWidget (QWidget* parent) :
    QTabWidget (parent)
    {
    QSettings   settings{ QSettings::UserScope };
    int         numControllers = settings.value ("NumControllers", 0).toInt ();

    m_controllers.reserve (numControllers);

    for (int ii = 0; ii < numControllers; ++ii)
        {
        utils::device::deviceInfo info;

        QString controller      = "Controller" + QString::number (ii);

        std::string type        = settings.value (controller + "/type",     "").toString    ()
                                                                               .toUtf8      ()
                                                                               .toStdString ();
        std::string name        = settings.value (controller + "/name",     "").toString    ()
                                                                               .toUtf8      ()
                                                                               .toStdString ();
        std::string proto       = settings.value (controller + "/proto",    "").toString    ()
                                                                               .toUtf8      ()
                                                                               .toStdString ();
        QString     transport   = settings.value (controller + "/transport","").toString    ();
        QString     address     = settings.value (controller + "/address",  "").toString    ();
        int         port        = settings.value (controller + "/port",      0).toInt       ();

        if ("TCP" == transport)
            {
            info.type = utils::device::TYPE_TCP;
            }
        else if ("UDP" == transport)
            {
            info.type = utils::device::TYPE_UDP;
            }
        else if ("SERIAL" == transport)
            {
            info.type = utils::device::TYPE_SERIAL;
            }

        utils::device::socketInfo skt;

        skt.addr = QHostAddress{ address };
        skt.port = port;

        info.info = skt;

        m_controllers.push_back (control::createController (type, name, proto, info));
        }

    control::controllerList controllersNonOwning;

    controllersNonOwning.reserve (numControllers);

    std::transform (m_controllers.begin (),
                    m_controllers.end (),
                    std::back_inserter (controllersNonOwning),
                    std::mem_fn (&std::unique_ptr<control::ControllerBase>::get));

    setUsesScrollButtons (false);

    addTab (m_trains = new trains::DualControlWidget{ controllersNonOwning, this },
            QIcon{ ":/icons/misc/train.svg" },
            "");
    addTab (m_actuators = new actuators::ActuatorPanel{ controllersNonOwning, this},
            QIcon{ ":/icons/misc/split.svg" },
            "");
    addTab (new sensors::SensorPanel{ this },
            QIcon{ ":/icons/misc/train-track.svg" },
            "");
    addTab (m_routes = new routes::RoutePanel{ controllersNonOwning, this },
            QIcon{ ":/icons/misc/path.svg" },
            "");
    addTab (new QWidget{ this },
            QIcon{ ":/icons/misc/binary.svg" },
            "");
    addTab (m_cfg = new config::ConfigPanel{ controllersNonOwning, this},
            QIcon{ ":/icons/misc/gear.svg" },
            "");

    setTabToolTip (0, "Trains");
    setTabToolTip (1, "Actuators");
    setTabToolTip (2, "Sensors");
    setTabToolTip (3, "Routes");
    setTabToolTip (4, "Programming");
    setTabToolTip (5, "Settings");

    setIconSize ({ 24, 24 });

    connect (m_cfg,
            &config::ConfigPanel::controllerDeleted,
             this,
            &MainWidget::controllerDeleted);

    connect (m_cfg,
             &config::ConfigPanel::controllerAdded,
             this,
             &MainWidget::controllerAdded);
    }

MainWidget::~MainWidget ()
    {
    QSettings   settings{ QSettings::UserScope };
    int         numControllers = static_cast<int> (m_controllers.size ());

    settings.setValue ("NumControllers", numControllers);

    m_controllers.reserve (numControllers);

    for (int ii = 0; ii < numControllers; ++ii)
        {
        utils::device::deviceInfo info  = m_controllers[ii]->getDeviceInfo ();
        utils::device::socketInfo skt   = std::get<utils::device::socketInfo> (info.info);

        QString controller      = "Controller" + QString::number (ii);
        QString transport;


        switch (info.type)
            {
            case utils::device::TYPE_SERIAL:
                {
                transport = "SERIAL";
                break;
                }
            case utils::device::TYPE_TCP:
                {
                transport = "TCP";
                break;
                }
            case utils::device::TYPE_UDP:
                {
                transport = "UDP";
                break;
                }
            }

        settings.setValue (controller + "/type",
                           m_controllers[ii]->getMetaClass ().name.c_str ());
        settings.setValue (controller + "/name",
                           m_controllers[ii]->getFriendlyName ().c_str ());
        settings.setValue (controller + "/proto",
                           m_controllers[ii]->getProtocol ().name.c_str ());
        settings.setValue (controller + "/transport",
                           transport);
        settings.setValue (controller + "/address",
                           skt.addr.toString ());
        settings.setValue (controller + "/port",
                           skt.port);
        }
    }

void MainWidget::controllerDeleted (control::ControllerBase& controller)
    {
    m_actuators->remove (controller);
    m_routes->remove (controller);

    utils::algorithm::eraseByPtr (m_controllers,
                                 &controller);
    }

void MainWidget::controllerAdded (const control::createControllerInfo& controllerInfo)
    {
    auto controller = control::createController (controllerInfo);

    if (NULL != controller)
        {
        auto& controllerRef = *m_controllers.emplace_back (std::move (controller)).get ();

        m_actuators->add (controllerRef);
        m_trains->add (controllerRef);
        m_cfg->addController (controllerRef);
        m_routes->add (controllerRef);
        }
    }

}