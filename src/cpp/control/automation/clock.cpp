/**
 * @file        automation/clock.cpp
 * @brief       Simulated time source with configurable start time and ratio wrt to real time
 * @author      Justin Scott
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <utils/os.hpp>

#include <control/automation/clock.hpp>

#include <QThread>

using namespace std::chrono_literals;

namespace control
{


FastClock::time_point FastClock::getTime () const
    {
    std::lock_guard lk{ m_mtx };
    return getTimeInternal ();
    }


void FastClock::setTime (const time_point& time)
    {
    std::lock_guard lk{ m_mtx };

    m_startTime         = time;
    m_startTimeSystem   = std::chrono::system_clock::now ();

    emit timeChanged (time);
    }


void FastClock::setRatio (uint ratio)
    {
    std::lock_guard lk{ m_mtx };

    m_startTime         = getTimeInternal ();
    m_startTimeSystem   = std::chrono::system_clock::now ();
    m_ratio             = ratio;
    }


void FastClock::setRunning (bool running)
    {
    std::lock_guard lk{ m_mtx };

    if (running && not m_running)
        {
        // starting, reset the system start time
        m_startTimeSystem = std::chrono::system_clock::now ();
        }

    if (not running && m_running)
        {
        // stopping, record the current time, so we can continue from there
        m_startTime = getTimeInternal ();
        emit stopped (m_startTime);
        }

    m_running = running;
    }


FastClock::FastClock () :
    m_thread (std::bind (&FastClock::tickingThread, this))
    {
    utils::os::setThreadName ("Fast Clock Thread", m_thread);
    }


FastClock::~FastClock ()
    {
    m_continue = false;
    m_thread.join ();
    }


void FastClock::tickingThread ()
    {
    time_point lastTick;

    while (m_continue)
        {
        if (m_running)
            {
            std::lock_guard         lk{ m_mtx };

            time_point              now             = getTimeInternal ();
            std::chrono::duration   timeSinceTick   = now - lastTick;

            // If the time is negative, the user has set the time back
            if (timeSinceTick.count () < 0)
                {
                lastTick = now;
                }
            else if (timeSinceTick >= 1s)
                {
                emit tick (now);
                lastTick = now;
                }
            }

        QThread::msleep (10);
        }
    }


FastClock::time_point FastClock::getTimeInternal () const
    {
    time_point current = m_startTime;

    if (m_running)
        {
        std::chrono::duration elapsed = std::chrono::system_clock::now () - m_startTimeSystem;

        current += elapsed * m_ratio;
        }

    return current;
    }


} // namespace control
