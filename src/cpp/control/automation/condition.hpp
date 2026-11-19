/**
 * @file        automation/condition.hpp
 * @brief       Represents an automated task
 * @author      Justin Scott
 * @date        2026-06-02
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/clock.hpp>

#include <layout/sensor.hpp>

#include <QString>

namespace control
{
///////////////////////////////////////////////////////////////////////////////
/// Time dependent condition to execute an automation
///
///////////////////////////////////////////////////////////////////////////////
class AutomationCondition : public QObject
    {
    Q_OBJECT
public:
    /// Type of automation condition
    enum type
        {
        TYPE_TIMER,         ///< Perform task every ... units of time
        TYPE_DAILY_ALARM,   ///< Perform task every day at a time
        TYPE_WEEKLY_ALARM,  ///< Perform task certain days of the week at a time
        TYPE_MONTHLY_ALARM, ///< Perform task one day of the month at a certain time
        TYPE_SENSOR,

        NUM_TYPES           ///< Delimiter only
        };

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if a component can be made into a condition
    ///
    /// @tparam     Component       Component type
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class Component>
    static constexpr bool isComponentCondition = std::is_same_v<Component, layout::SensorModule>;

    ///////////////////////////////////////////////////////////////////////////////
    /// Virtual destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~AutomationCondition () {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Evaluate the condition
    ///
    /// @param[in]  lastTimeChecked     Last time this was evaluated
    /// @param[in]  lastTimeRun         Last time this condition executed
    /// @param[in]  now                 Current simulated time
    ///
    /// @return     True if expression evaluates to true
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool evaluate (const FastClock::time_point& lastTimeChecked,
                           const FastClock::time_point& lastTimeRun,
                           const FastClock::time_point& now) const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the human readable description of this condition
    ///
    /// @return     Description text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual QString description () const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the condition type
    ///
    /// @return     Condition type
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual type getType () const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if this condition relies on a component
    ///
    /// @param[in]  component       Component to check
    ///
    /// @return     true if it does
    ///
    /// @remarks    Default implementation is always false, since most don't use
    ///             components
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool isAutomating (const layout::ComponentBase& component) const { return false; }

signals:
    void destroyed ();
    };


///////////////////////////////////////////////////////////////////////////////
/// Timer condition. Executes a task after a fixed period of time
///
///////////////////////////////////////////////////////////////////////////////
class AutomationTimer : public AutomationCondition
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constuctor
    ///
    /// @tparam     Ratio           Timer ratio
    ///
    /// @param[in]  duration        Timer duration
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class Ratio>
    explicit AutomationTimer (std::chrono::duration<FastClock::time_point::rep, Ratio> duration) :
        m_duration (std::chrono::duration_cast<std::chrono::seconds> (duration))
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Evaluate the timer
    ///
    /// @param[in]  lastTimeChecked     Last time this was evaluated
    /// @param[in]  lastTimeRun         Last time this condition executed
    /// @param[in]  now                 Current simulated time
    ///
    /// @return     True if time has completed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool evaluate (const FastClock::time_point& lastTimeChecked,
                           const FastClock::time_point& lastTimeRun,
                           const FastClock::time_point& now) const override
        { return now - lastTimeRun >= m_duration; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the human readable description of this timer
    ///
    /// @return     Description text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual QString description () const override
        {
        auto [count, unit] = utils::time::convertToBestUnit (m_duration, utils::time::ALL_UNITS);

        return QObject::tr ("Every %1").arg (utils::time::formatWithUnit (count, unit));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the condition type
    ///
    /// @return     TYPE_TIMER
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual type getType () const override { return TYPE_TIMER; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get this timer's duration
    ///
    /// @return     Timer duration
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::chrono::seconds getDuration () const { return m_duration; }

private:
    std::chrono::seconds m_duration;    ///< Timer duration
    };


///////////////////////////////////////////////////////////////////////////////
/// Alarm condition. Executes the condition at a given time every day
///
///////////////////////////////////////////////////////////////////////////////
class AutomationAlarm : public AutomationCondition
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  time        Alarm time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit AutomationAlarm (const QTime& time) :
        m_time (time)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Evaluate the alarm
    ///
    /// @param[in]  lastTimeChecked     Last time this was evaluated
    /// @param[in]  lastTimeRun         Last time this condition executed
    /// @param[in]  now                 Current simulated time
    ///
    /// @return     True if alarm has passed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool evaluate (const FastClock::time_point& lastTimeChecked,
                           const FastClock::time_point& lastTimeRun,
                           const FastClock::time_point& now) const override
        {
        QDateTime   qAlarmToday{ utils::time::fromTP (now).date (), m_time };
        auto        alarmToday = utils::time::toTP (qAlarmToday);

        return lastTimeChecked <  alarmToday &&
                           now >= alarmToday;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the human readable description of this alarm
    ///
    /// @return     Description text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual QString description () const override
        { return QObject::tr ("Each day at %1").arg (m_time.toString ()); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the condition type
    ///
    /// @return     TYPE_DAILY_ALARM
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual type getType () const override { return TYPE_DAILY_ALARM; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the time the alarm goes off each
    ///
    /// @return     Alarm time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QTime getTime () const { return m_time; }

private:
    QTime m_time;   ///< Time alarm goes off
    };


///////////////////////////////////////////////////////////////////////////////
/// Alarm that goes off at the same time, but only certain days of the week
///
///////////////////////////////////////////////////////////////////////////////
class AutomationWeeklyAlarm : public AutomationAlarm
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  time    Alarm time
    /// @param[in]  days    Which days the alarm should trigger
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationWeeklyAlarm (const QTime& time, const utils::time::days& days) :
        AutomationAlarm (time),
        m_days (days)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Evaluate the alarm
    ///
    /// @param[in]  lastTimeChecked     Last time this was evaluated
    /// @param[in]  lastTimeRun         Last time this condition executed
    /// @param[in]  now                 Current simulated time
    ///
    /// @return     True if alarm has passed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool evaluate (const FastClock::time_point& lastTimeChecked,
                           const FastClock::time_point& lastTimeRun,
                           const FastClock::time_point& now) const override
        {
        QDate today = utils::time::fromTP (now).date ();

        return m_days[today.dayOfWeek () - 1] &&
               AutomationAlarm::evaluate (lastTimeChecked,
                                          lastTimeRun,
                                          now);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the human readable description of this alarm
    ///
    /// @return     Description text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual QString description () const override
        { return QObject::tr ("%1 at %2").arg (utils::time::formatDaysOfTheWeek (m_days),
                                               getTime ().toString ()); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the condition type
    ///
    /// @return     TYPE_WEEKLY_ALARM
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual type getType () const override { return TYPE_WEEKLY_ALARM; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get which days this alarm is triggered on
    ///
    /// @return     Days bitset
    ///
    ///////////////////////////////////////////////////////////////////////////////
    utils::time::days getDays () const { return m_days; }
private:
    utils::time::days   m_days; ///< Selected days
    };

///////////////////////////////////////////////////////////////////////////////
/// Alarm that goes off at a certain time, one day of the month
///
///////////////////////////////////////////////////////////////////////////////
class AutomationMonthlyAlarm : public AutomationAlarm
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  time        Alarm time
    /// @param[in]  dayOfMonth  Which day of the month the alarm should trigger [1,31]
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationMonthlyAlarm (const QTime& time, uint8_t dayOfMonth) :
        AutomationAlarm (time),
        m_dayOfMonth (dayOfMonth)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Evaluate the alarm
    ///
    /// @param[in]  lastTimeChecked     Last time this was evaluated
    /// @param[in]  lastTimeRun         Last time this condition executed
    /// @param[in]  now                 Current simulated time
    ///
    /// @return     True if alarm has passed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool evaluate (const FastClock::time_point& lastTimeChecked,
                           const FastClock::time_point& lastTimeRun,
                           const FastClock::time_point& now) const override
        {
        QDate today = utils::time::fromTP (now).date ();

        return today.day () == m_dayOfMonth &&
               AutomationAlarm::evaluate (lastTimeChecked,
                                          lastTimeRun,
                                          now);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the human readable description of this alarm
    ///
    /// @return     Description text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual QString description () const override
        { return QObject::tr ("Day % of every month at %2").arg (m_dayOfMonth,
                                                                 getTime ().toString ()); }
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the condition type
    ///
    /// @return     TYPE_MONTHLY_ALARM
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual type getType () const override { return TYPE_MONTHLY_ALARM; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get which day of the month the alarm triggers
    ///
    /// @return     Day of the month [1,31]
    ///
    ///////////////////////////////////////////////////////////////////////////////
    uint8_t getDayOftheMonth () const { return m_dayOfMonth; }

private:
    uint8_t m_dayOfMonth;   ///< Alarm day
    };

///////////////////////////////////////////////////////////////////////////////
/// Track sensor condition
///
///////////////////////////////////////////////////////////////////////////////
class AutomationSensor : public AutomationCondition
    {
public:
    /// What sensor change triggers this automation
    enum condition
        {
        CONDITION_RISING_EDGE,  ///< Sensor goes from unoccupied to occupied
        CONDITION_FALLING_EDGE, ///< Sensor goes from occupied to unoccupied
        CONDITION_CHANGE,       ///< Any change to the sensor happens

        NUM_CONDITIONS          ///< Delimiter only
        };

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  mod     Sensor module
    /// @param[in]  cond    Sensor change
    /// @param[in]  port    Port number of module
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationSensor (layout::SensorModule  mod,
                      condition             cond,
                      uint8_t               port) :
        m_module (std::move (mod)),
        m_condition (cond),
        m_port (port),
        m_lastState (m_module[port])
        {
        connect (&m_module,
                &layout::SensorModule::destroyed,
                 this,
                &AutomationCondition::destroyed);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Evaluate the sensor condition
    ///
    /// @param[in]  lastTimeChecked     Last time this was evaluated
    /// @param[in]  lastTimeRun         Last time this condition executed
    /// @param[in]  now                 Current simulated time
    ///
    /// @return     True if sensor condition has been met
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool evaluate (const FastClock::time_point& lastTimeChecked,
                           const FastClock::time_point& lastTimeRun,
                           const FastClock::time_point& now) const override
        {
        bool newState   = m_module[m_port];
        bool triggered  = false;

        switch (m_condition)
            {
            case CONDITION_CHANGE:
                {
                triggered = m_lastState != newState;
                break;
                }

            case CONDITION_FALLING_EDGE:
                {
                triggered = true  == m_lastState &&
                            false == newState;
                break;
                }
            case CONDITION_RISING_EDGE:
                {
                triggered = false == m_lastState &&
                            true  == newState;
                break;
                }
            }

        m_lastState = newState;

        return triggered;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the human readable description of this condition
    ///
    /// @return     Description text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual QString description () const override
        {
        const QString CONDITIONS[] =
            {
            QObject::tr ("Port %1 of %2 becomes occupied"),
            QObject::tr ("Port %1 of %2 becomes unoccupied"),
            QObject::tr ("Port %1 of %2 changes")
            };

        return CONDITIONS[m_condition].arg (m_port, m_module.getName ());
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the condition type
    ///
    /// @return     TYPE_SENSOR
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual type getType () const override { return TYPE_SENSOR; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if this condition relies on a sensor module
    ///
    /// @param[in]  component       Sensor module to check
    ///
    /// @return     true if it does
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool isAutomating (const layout::ComponentBase& component) const override
        {
        const layout::SensorModule* mod = dynamic_cast<const layout::SensorModule*> (&component);
        return NULL != mod && *mod == m_module;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the module used in this condition
    ///
    /// @return     Sensor module
    ///
    ///////////////////////////////////////////////////////////////////////////////
    layout::SensorModule getModule () const { return m_module; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the port used to trigger this condition
    ///
    /// @return     Port number
    ///
    ///////////////////////////////////////////////////////////////////////////////
    uint8_t getPort () const { return m_port; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the sensor condition type
    ///
    /// @return     Condition type
    ///
    ///////////////////////////////////////////////////////////////////////////////
    condition getCondition () const { return m_condition; }
private:
    const layout::SensorModule  m_module;       ///< Sensor module we are listening for
    const condition             m_condition;    ///< Sensor change we are waiting for
    const uint8_t               m_port;         ///< Which port in the sensor
    mutable bool                m_lastState;    ///< The last sensor state
    };

///////////////////////////////////////////////////////////////////////////////
/// Get the name of an enumerate condition type
///
/// @param[in]  type    Condition type
///
/// @return     Condition type text
///
///////////////////////////////////////////////////////////////////////////////
inline QString conditionText (AutomationCondition::type type)
    {
    const QString CONDITIONS[] =
        {
        QObject::tr ("Timer"),
        QObject::tr ("Daily Alarm"),
        QObject::tr ("Weekly Alarm"),
        QObject::tr ("Monthly Alarm"),
        QObject::tr ("Track Sensor")
        };
    ASSERT_ARRAY_LENGTH (CONDITIONS, AutomationCondition::NUM_TYPES);

    return CONDITIONS[type];
    }

///////////////////////////////////////////////////////////////////////////////
/// Get the sensor condition text
///
/// @return     Text for a sensor condition
///
///////////////////////////////////////////////////////////////////////////////
inline QString sensorConditionText (AutomationSensor::condition type)
    {
    const QString CONDITIONS[] =
        {
        QObject::tr ("Rising Edge"),
        QObject::tr ("Falling Edge"),
        QObject::tr ("Any Change")
        };
    ASSERT_ARRAY_LENGTH (CONDITIONS, AutomationSensor::NUM_CONDITIONS);

    return CONDITIONS[type];
    }


} // namespace control
