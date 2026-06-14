/**
 * @file        automation/task.hpp
 * @brief       Represents an automated task
 * @author      Justin Scott
 * @date        2026-06-02
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/automation/clock.hpp>
#include <control/automation/item.hpp>
#include <control/automation/condition.hpp>

#include <QObject>

namespace control
{

///////////////////////////////////////////////////////////////////////////////
/// Represents a task that has been automated
///
///////////////////////////////////////////////////////////////////////////////
class AutomationTask : public QObject
    {
    Q_OBJECT
public:
    AutomationItem::action                  action;     ///< Action to perform on item
    std::unique_ptr<AutomationCondition>    condition;  ///< Condition to execute task
    bool                                    enabled;    ///< True if task is enabled
    bool                                    doOnce;     ///< If true, task disables itself
                                                        ///  after the first time it executes
    QString                                 name;       ///< Name of the task for the UI

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  now     Creation time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit AutomationTask (const FastClock::time_point& now) :
        m_lastChecked (now),
        m_lastRun (now)
        {}


    ///////////////////////////////////////////////////////////////////////////////
    /// Evaluate the task for a given time point
    ///
    /// @param[in]  now     Current simulated time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void doTask (const FastClock::time_point& now);

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the item to automate. This task takes ownership of the item
    ///
    /// @param[in]  item        Item to automate
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setItem (AutomationItem* item);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the automated item
    ///
    /// @return     Automated item
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationItem* getItem () { return m_item; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get a description of this task
    ///
    /// @return     Formatted desription text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QString description () const;

signals:
    ///////////////////////////////////////////////////////////////////////////////
    /// Signals the item this is automating has been destroyed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void destroyed ();

private:
    FastClock::time_point   m_lastChecked;      ///< Last time doTask() was called
    FastClock::time_point   m_lastRun;          ///< Last time doTask() ran item
    AutomationItem*         m_item  = NULL;     ///< Automated item

    };

} // namespace control