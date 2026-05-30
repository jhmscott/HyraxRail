/**
 * @file        controller/worker.cpp
 * @brief       Network communication worker thread for
 *              model train control
 * @author      Justin Scott
 * @date        2026-01-11
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/controllers/worker.hpp>

#include <utils/os.hpp>

#include <sstream>

namespace control
{
ConnectionWorkerThread::ConnectionWorkerThread (std::string_view                controllerName,
                                                std::unique_ptr<ProtocolBase>&& proto) :
    m_proto (std::move (proto)),
    m_thread (std::bind (&ConnectionWorkerThread::loop, this))
    {
    std::stringstream ss;

    ss << controllerName << " Network Worker Thread";

    utils::os::setThreadName (ss.str (), m_thread);

    if (m_proto->isNetworkConnection ())
        {
        m_pinger = new utils::Pinger{ m_proto->getIpAddress () };
        }

    m_cv.notify_all ();

    }

ConnectionWorkerThread::~ConnectionWorkerThread ()
    {
    try
        {
        std::unique_lock lk (m_mtx);

        m_continue = false;

        // Wake up the thread to clear it's thread and exit;
        m_cv.notify_all ();

        lk.unlock ();

        // Wait for the thread to exit
        m_thread.join ();
        }
    catch (...)
        {
        // Have to swallow this exception because it's a destructor
        }

    if (NULL != m_pinger)
        {
        delete m_pinger;
        }
    }

void ConnectionWorkerThread::loop ()
    {
    using namespace std::chrono_literals;

    const std::chrono::duration pingInterval     = 5000ms;
    const std::chrono::duration queueTimeout     = 1000ms;

    std::unique_lock lk (m_mtx);

    std::chrono::time_point<std::chrono::system_clock> lastPing;
    std::chrono::time_point<std::chrono::system_clock> lastEvent;

    while (m_continue)
        {
        std::chrono::duration   eventInterval   = m_proto->getEventPollInterval ();
        std::chrono::duration   waitInterval    = std::min (eventInterval, queueTimeout);

        m_cv.wait_for (lk, waitInterval);

        health                  newHealth       = m_health;

        if (m_proto->maintainConnection ())
            {
            if (NULL != m_pinger)
                {
                if (std::chrono::system_clock::now () - lastPing >= pingInterval)
                    {
                    auto res = m_pinger->ping ();

                    if (res.roundtrip <= 1ms)
                        {
                        newHealth.level = HEALTH_FULL;
                        }
                    else if (res.roundtrip <= 5ms)
                        {
                        newHealth.level = HEALTH_HIGH;
                        }
                    else if (res.roundtrip <= 10ms)
                        {
                        newHealth.level = HEALTH_MEDIUM;
                        }
                    else // (res.roundtrip > 10ms)
                        {
                        newHealth.level = HEALTH_LOW;
                        }

                    newHealth.ping = res.roundtrip;

                    lastPing = std::chrono::system_clock::now ();
                    }
                }
            else
                {
                newHealth = { HEALTH_CONNECTED, std::chrono::milliseconds{ 0 } };
                }

            if (std::chrono::system_clock::now () - lastEvent >= eventInterval)
                {
                lastEvent = std::chrono::system_clock::now ();
                }

            while (!m_taskQueue.empty ())
                {
                m_taskQueue.front ()->callContained (*m_proto);
                m_taskQueue.pop ();
                }
            }
        else
            {
            if (NULL == m_pinger)
                {
                newHealth = { HEALTH_DISCONNECTED, std::chrono::milliseconds{ 0 } };
                }
            else // If com port...
                {
                newHealth = { HEALTH_DEAD, std::chrono::milliseconds{ 0 } };
                }

            while (not m_taskQueue.empty () &&
                   std::chrono::system_clock::now () -
                   m_taskQueue.front ()->getCreationTime () > queueTimeout)
                {
                m_taskQueue.pop ();
                }
            }

        if (newHealth != m_health)
            {
            std::lock_guard lk{ m_healthLock };
            m_health = newHealth;
            m_healthChange.notify_all ();
            }
        }
    }

}
