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
ConnectionWorkerThread::ConnectionWorkerThread (std::string_view controllerName, std::unique_ptr<ProtocolBase>&& proto) :
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

    std::unique_lock lk (m_mtx);

    std::chrono::time_point<std::chrono::system_clock> lastPing;
    std::chrono::time_point<std::chrono::system_clock> lastEvent;

    while (m_continue)
        {
        std::chrono::duration eventInterval = m_proto->getEventPollInterval ();
        std::chrono::duration waitInterval  = std::min (eventInterval, pingInterval);

        m_cv.wait_for (lk, waitInterval);

        if (m_proto->maintainConnection ())
            {
            if (NULL != m_pinger)
                {
                if (std::chrono::system_clock::now () - lastPing >= pingInterval)
                    {
                    auto res = m_pinger->ping ();

                    if (res.roundtrip <= 1ms)
                        {
                        m_health.level = HEALTH_FULL;
                        }
                    else if (res.roundtrip <= 5ms)
                        {
                        m_health.level = HEALTH_HIGH;
                        }
                    else if (res.roundtrip <= 10ms)
                        {
                        m_health.level = HEALTH_MEDIUM;
                        }
                    else // (res.roundtrip > 10ms)
                        {
                        m_health.level = HEALTH_LOW;
                        }

                    m_health.ping = res.roundtrip;

                    lastPing = std::chrono::system_clock::now ();
                    }
                }
            else
                {
                m_health = { HEALTH_CONNECTED, std::chrono::milliseconds{ 0 } };
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
        // If com port...
        else if (NULL == m_pinger)
            {
            m_health = { HEALTH_DISCONNECTED, std::chrono::milliseconds{ 0 } };
            }
        else
            {
            m_health = { HEALTH_DEAD, std::chrono::milliseconds{ 0 } };
            }
        }
    }

}
