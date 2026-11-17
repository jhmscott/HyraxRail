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
    AutomationItem::action  action;     ///< Action to perform on item
    bool                    enabled;    ///< True if task is enabled
    bool                    doOnce;     ///< If true, task disables itself
                                        ///  after the first time it executes
    QString                  name;      ///< Name of the task for the UI

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
    /// Get the automated item
    ///
    /// @return     Automated item
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const AutomationItem* getItem () const { return m_item; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the automation condition. This task takes ownership of the item
    ///
    /// @param[in]  condition       Condition to execute the item in the task
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setCondition (AutomationCondition* condition);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the automation condition
    ///
    /// @return     Automation condition
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationCondition* getCondition () { return m_condition; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the automated item
    ///
    /// @return     Automated item
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const AutomationCondition* getCondition () const { return m_condition; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get a description of this task
    ///
    /// @return     Formatted desription text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QString description () const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if this task is automating a given Component
    ///
    /// @tparam     Component       Component type
    ///
    /// @param[in]  component       Component to check
    ///
    /// @return     True if this item is automating component
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class Component>
    bool isAutomating (const Component& component) const
        {
        static_assert (AutomationItem::isAutomatable<Component> ||
                       AutomationCondition::isComponentCondition<Component>,
                       "Component type not usable by condition or item");
        bool automated = false;

        // I may someday have a componponent type that can be a condition or item
        // So leaving this as if not else if

        if constexpr (AutomationItem::isAutomatable<Component>)
            {
            automated = NULL != m_item && m_item->isAutomating (component);
            }

        if constexpr (AutomationCondition::isComponentCondition<Component>)
            {
            automated = automated || (NULL != m_condition && m_condition->isAutomating (component));
            }

        return automated;
        }

signals:
    ///////////////////////////////////////////////////////////////////////////////
    /// Signals the item this is automating has been destroyed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void destroyed ();

private:
    FastClock::time_point   m_lastChecked;      ///< Last time doTask() was called
    FastClock::time_point   m_lastRun;          ///< Last time doTask() ran item
    AutomationItem*         m_item      = NULL; ///< Automated item
    AutomationCondition*    m_condition = NULL; ///< Condition to execute task

    };

} // namespace control
