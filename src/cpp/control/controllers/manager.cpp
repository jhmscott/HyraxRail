/**
 * @file        controller/manager.cpp
 * @brief       Provides container to manage controllers, with signals for
 *              when controllers are added, deleted or changed
 * @author      Justin Scott
 * @date        2026-04-20
 *
 * @copyright   Copyright (c) 2026 Justin ScottS
 */

#include <control/controllers/manager.hpp>

#include <utils/algorithm.hpp>

#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QSettings>

namespace control
{
ControllerManager::ControllerManager (QObject* parent) :
    QObject (parent),
    m_timer (new QTimer{ this })
    {
    connect (m_timer,
            &QTimer::timeout,
             this,
            &ControllerManager::refreshConnectionHealth);
    m_timer->start (500);
    }

void ControllerManager::append (const createControllerInfo& info, bool block)
    {
    auto controller = createController (info);

    if (NULL != controller)
        {
        m_controllers.emplace_back (controllerInstance{ std::move (controller), {} });

        if (block)
            {
            m_controllers.back ().controller->refresh ();
            emit controllerAdded (*m_controllers.back ().controller);
            }
        else
            {
            QFutureWatcher<void>* watcher = new QFutureWatcher<void>;
            auto& newController = *m_controllers.back ().controller;

            connect (watcher,
                    &QFutureWatcher<void>::finished,
                    this,
                    [this, &newController, watcher] () -> void
                    {
                    emit controllerAdded (newController);
                    delete watcher;
                    });

            watcher->setFuture (QtConcurrent::run (
                utils::algorithm::bindMemFn (&ControllerBase::refresh, newController)));
            }
        }
    }

void ControllerManager::remove (const ControllerBase& controller)
    {
    auto it = findController (controller);

    if (m_controllers.end () != it)
        {
        erase (m_controllers.begin () + (it - m_controllers.cbegin ()));
        }
    }

void ControllerManager::refresh (const ControllerBase& controller)
    {
    auto it = findController (controller);

    if (m_controllers.end () != it)
        {
        QFutureWatcher<void>* watcher = new QFutureWatcher<void>;

        connect (watcher,
                &QFutureWatcher<void>::finished,
                 this,
                [this, &controller, watcher] () -> void
                {
                emit cacheRebuilt (controller);
                delete watcher;
                });

        emit refreshStarted (controller);

        watcher->setFuture (QtConcurrent::run (
            utils::algorithm::bindMemFn (&ControllerBase::refresh, *it->controller)));
        }
    }

void ControllerManager::erase (Iterator it)
    {
    size_t idx = std::distance (m_controllers.begin (), it.m_it);

    emit controllerDeleted (*m_controllers[idx].controller);

    m_controllers.erase (it.m_it);
    }

ptrdiff_t ControllerManager::indexOf (const ControllerBase& controller) const
    {
    ptrdiff_t   idx = -1;
    auto        it  =  findController(controller);

    if (m_controllers.end () != it)
        {
        idx = std::distance (m_controllers.begin (), it);
        }

    return idx;
    }

void ControllerManager::clear ()
    {
    for (const ControllerBase& controller : *this)
        {
        emit controllerDeleted (controller);
        }

    m_controllers.clear ();
    }

void ControllerManager::refreshConnectionHealth ()
    {
    for (auto& inst : m_controllers)
        {
        const auto newHealth = inst.controller->getConnectionHealth ();

        if (inst.lastHealth != newHealth)
            {
            emit connectionHealthChanged (*inst.controller);
            }

        if (inst.lastHealth.isConnected () && not newHealth.isConnected ())
            {
            emit disconnected (*inst.controller);
            }

        if (not inst.lastHealth.isConnected () && newHealth.isConnected ())
            {
            emit reconnected (*inst.controller);
            refresh (*inst.controller);
            }

        inst.lastHealth = newHealth;
        }
    }

ControllerManager::controllerList::iterator
ControllerManager::findController (const ControllerBase& controller)
    {
    return std::find_if (m_controllers.begin (),
                         m_controllers.end (),
                         [&controller] (const controllerInstance& inst)
                         { return inst.controller.get () == &controller; });
    }

ControllerManager::controllerList::const_iterator
ControllerManager::findController (const ControllerBase& controller) const
    {
    return std::find_if (m_controllers.begin (),
                         m_controllers.end (),
                         [&controller] (const controllerInstance& inst)
                         { return inst.controller.get () == &controller; });
    }


std::unique_ptr<ControllerManager>
loadControllersFromUserSettings (std::function<void (const std::string&)> progress)
    {
    std::unique_ptr<ControllerManager> manager{ new ControllerManager };


    QSettings   settings{ QSettings::UserScope };
    int         numControllers = settings.value ("NumControllers", 0).toInt ();


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
        QString     host        = settings.value (controller + "/host",     "").toString    ();
        int         port        = settings.value (controller + "/port",      0).toInt       ();
        int         hostType    = settings.value (controller + "/hostType",  0).toInt       ();

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

        skt.host = utils::device::HostInfo::fromString (host,
                                                       static_cast<utils::device::HostInfo::type> (hostType));
        skt.port = port;

        info.info = skt;

        if (progress)
            {
            progress (name);
            }

        manager->append ({ type, name, proto, info }, true); // block
        }

    return std::move (manager);
    }

void saveControllersToUserSettings (const ControllerManager& controllers)
    {
    QSettings   settings{ QSettings::UserScope };
    int         numControllers = static_cast<int> (controllers.size ());

    settings.setValue ("NumControllers", numControllers);

    for (int ii = 0; ii < numControllers; ++ii)
        {
        utils::device::deviceInfo info  = controllers[ii].getDeviceInfo ();
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
            default:
                {
                qWarning () << "Unexpected device type";
                break;
                }
            }

        settings.setValue (controller + "/type",
                          controllers[ii].getMetaClass ().name.c_str ());
        settings.setValue (controller + "/name",
                          controllers[ii].getFriendlyName ().c_str ());
        settings.setValue (controller + "/proto",
                          controllers[ii].getProtocol ().name.c_str ());
        settings.setValue (controller + "/transport",
                          transport);
        settings.setValue (controller + "/host",
                          skt.host.toString ());
        settings.setValue (controller + "/hostType",
                          static_cast<int> (skt.host.getType ()));
        settings.setValue (controller + "/port",
                          skt.port);
        }
    }
} // namespace control