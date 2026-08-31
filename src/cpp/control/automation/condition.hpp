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

#include <QString>

namespace control
{
///////////////////////////////////////////////////////////////////////////////
/// Time dependent condition to execute an automation
///
///////////////////////////////////////////////////////////////////////////////
class AutomationCondition
    {
public:
    // Type of automation condition
    enum type
        {
        TYPE_TIMER,         ///< Perform task every ... units of time
        TYPE_DAILY_ALARM,   ///< Perform task every day at a time
        TYPE_WEEKLY_ALARM,  ///< Perform task certain days of the week at a time
        TYPE_MONTHLY_ALARM, ///< Perform task one day of the month at a certain time

        NUM_TYPES           ///< Delimiter only
        };

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
    explicit AutomationTimer (std::chrono::duration<long long, Ratio> duration) :
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
    utils::time::days   m_days;
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
        QObject::tr ("Monthly Alarm")
        };
    ASSERT_ARRAY_LENGTH (CONDITIONS, AutomationCondition::NUM_TYPES);

    return CONDITIONS[type];
    }

} // namespace control