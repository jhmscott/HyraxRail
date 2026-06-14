/**
 * @file        automation/manager.cpp
 * @brief       Container for a set of automations
 * @author      Justin Scott
 * @date        2026-06-02
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/automation/manager.hpp>

namespace control
{

AutomationManager::AutomationManager (QObject* parent) :
    QObject (parent)
    {
    connect (&FastClock::instance (),
             &FastClock::tick,
              this,
             &AutomationManager::reEvaluate);

    connect (&FastClock::instance (),
             &FastClock::timeChanged,
              this,
             &AutomationManager::timeChange);
    }

void AutomationManager::taskItemDestroyed ()
    {
    QObject* task = sender ();

    auto it = std::find_if (begin (),
                            end (),
                            [task] (const AutomationTask& other) -> bool
                            { return task == &other; });

    if (end () != it)
        {
        erase (it);
        }
    }


AutomationTask& AutomationManager::emplace ()
    {
    auto& task = m_tasks.emplace_back (FastClock::now ());

    connect (&task,
             &AutomationTask::destroyed,
              this,
             &AutomationManager::taskItemDestroyed);

    return task;
    }

void AutomationManager::erase (const_iterator it)
    {
    emit taskDestroyed (*it);
    m_tasks.erase (it);
    }

void AutomationManager::reEvaluate (const FastClock::time_point& now)
    {
    for (auto& task : m_tasks)
        {
        task.doTask (now);
        }
    }


void AutomationManager::timeChange (const FastClock::time_point& newTime)
    {
    // TODO: Adjust automations
    }

} // namespace control