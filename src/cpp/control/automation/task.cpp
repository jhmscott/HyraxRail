/**
 * @file        automation/task.cpp
 * @brief       Represents an automated task
 * @author      Justin Scott
 * @date        2026-06-02
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/automation/task.hpp>

namespace control
{
void AutomationTask::doTask (const FastClock::time_point& now)
    {
    if (enabled)
        {
        if (m_condition->evaluate (m_lastChecked,
                                   m_lastRun,
                                   now))
            {
            m_item->doAction (action);

            m_lastRun = now;

            if (doOnce)
                {
                enabled = false;
                }
            }

        m_lastChecked = now;
        }
    }

void AutomationTask::setItem (AutomationItem* item)
    {
    if (NULL != m_item)
        {
        delete m_item;
        }

    if (NULL != item)
        {
        m_item = item;
        m_item->setParent (this);

        connect (m_item,
                &AutomationItem::destroyed,
                 this,
                &AutomationTask::destroyed);
        }
    }

void AutomationTask::setCondition (AutomationCondition* condition)
    {
    if (NULL != m_condition)
        {
        delete m_condition;
        }

    if (NULL != condition)
        {
        m_condition = condition;
        m_condition->setParent (this);

        connect (m_condition,
                &AutomationCondition::destroyed,
                 this,
                &AutomationTask::destroyed);
        }
    }

QString AutomationTask::description () const
    {
    return QString{ "%1, %2 %3" }.arg (m_condition->description (),
                                       actionText (action),
                                       m_item->name ());
    }

} // namespace control
