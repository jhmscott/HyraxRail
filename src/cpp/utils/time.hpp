/**
 * @file        utils/time.hpp
 * @brief       Time utilities
 * @author      Justin Scott
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <common.hpp>

#include <QDateTime>
#include <QTimeZone>

#include <bitset>
#include <chrono>

namespace utils::time
{

enum class dateFormat
    {

    };

// Enumerated months of the year
// Note that this starts at 0, while QDate::month() starts at 1
//
enum month
    {
    JANUARY,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER,

    NUM_MONTHS
    };

// Enumerated days of the week
// Note that this starts at 0, while QDate::dayOfWeek() starts at 1
//
enum dayOfTheWeek
    {
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY,

    NUM_DAYS
    };

// Bitset containing a set of days in an arbitrary week
using days = std::bitset<NUM_DAYS>;

enum unit
    {
    NANOSECONDS,
    MICROSECONDS,
    MILLISECONDS,
    SECONDS,
    MINUTES,
    HOURS,
    DAYS,
    WEEEKS,

    NUM_UNITS
    };

using units = std::bitset<NUM_UNITS>;

inline constexpr units allUnits{ (1 << NUM_UNITS) - 1 };

// System clock time point type
template<class Duration>
using system_time = std::chrono::time_point<std::chrono::system_clock, Duration>;


namespace internal
{
///////////////////////////////////////////////////////////////////////////////
/// Fake class used to group time strings together in QT Linguist
///
///////////////////////////////////////////////////////////////////////////////
class TimeStringConstants : public QObject
    {
Q_OBJECT
    };
} // namespace internal

///////////////////////////////////////////////////////////////////////////////
/// Create a QT date time object from an STL system clock time point
///
/// @tparam     Duration    Duration of time point
///
/// @param[in]  tp          Time point
///
/// @return     Date time object for tp
///
///////////////////////////////////////////////////////////////////////////////
template<class Duration>
[[nodiscard]] QDateTime fromTP (const system_time<Duration>& tp)
    {
    return QDateTime::fromStdTimePoint (
                std::chrono::time_point_cast<std::chrono::milliseconds> (tp));
    }

///////////////////////////////////////////////////////////////////////////////
/// Convert a QT date time objext to aN STL system clock time point
///
/// @tparam     Duration    Duration of time point
///
/// @param[in]  time        QT date time
///
/// @return     time as time point
///
///////////////////////////////////////////////////////////////////////////////
template<class Duration = std::chrono::system_clock::duration>
[[nodiscard]] system_time<Duration> toTP (const QDateTime& time)
    {
    return system_time<Duration>{
                std::chrono::duration_cast<Duration>
                    (std::chrono::milliseconds{
                        time.toMSecsSinceEpoch () }) };
    }

///////////////////////////////////////////////////////////////////////////////
/// Convert QT date and time objects to a time point
///
/// @tparam     Duration    (optional) time point duration. Defaults to the
///                         duration used by the system clock
///
/// @param[in]  date        Date component
/// @param[in]  time        Time component
///
/// @return     date + time as a time point
///
///////////////////////////////////////////////////////////////////////////////
template<class Duration = std::chrono::system_clock::duration>
[[nodiscard]] system_time<Duration> toTP (const QDate& date, const QTime& time)
    {
    return toTP (QDateTime{ date, time, QTimeZone::utc () });
    }

///////////////////////////////////////////////////////////////////////////////
/// Get the name of the day of a week, translated into the current locale
///
/// @param[in]  day     Day of the week
///
/// @return     Day of the week text
///
///////////////////////////////////////////////////////////////////////////////
inline QString dayOfWeekText (dayOfTheWeek day)
    {
    using namespace internal;

    const QString DAYS[] =
        {
        TimeStringConstants::tr ("Monday"),
        TimeStringConstants::tr ("Tuesday"),
        TimeStringConstants::tr ("Wednesday"),
        TimeStringConstants::tr ("Thursday"),
        TimeStringConstants::tr ("Friday"),
        TimeStringConstants::tr ("Saturday"),
        TimeStringConstants::tr ("Sunday")
        };
    ASSERT_ARRAY_LENGTH (DAYS, NUM_DAYS);

    return DAYS[day];
    }

///////////////////////////////////////////////////////////////////////////////
/// Get the translated abbreviation for a day of the week
///
/// @param[in]  day     Which day to get
///
/// @return     Day of the week abreviation
///
/// @remarks    Unlike QLocale::dayName(), the translations are all three
///             characters
///
///////////////////////////////////////////////////////////////////////////////
inline QString dayOfWeekAbreviation (dayOfTheWeek day)
    {
    using namespace internal;

    const QString DAYS[] =
        {
        TimeStringConstants::tr ("Mon"),
        TimeStringConstants::tr ("Tue"),
        TimeStringConstants::tr ("Wed"),
        TimeStringConstants::tr ("Thu"),
        TimeStringConstants::tr ("Fri"),
        TimeStringConstants::tr ("Sat"),
        TimeStringConstants::tr ("Sun")
        };
    ASSERT_ARRAY_LENGTH (DAYS, NUM_DAYS);

    return DAYS[day];
    }

///////////////////////////////////////////////////////////////////////////////
/// Get the translated abreviation for a month
///
/// @param[in]  mon     Month
///
/// @return     Month abreviation
///
/// @remarks    Unlike QLocale::monthName(), the translations are all three
///             characters
///
///////////////////////////////////////////////////////////////////////////////
inline QString monthAbreviation (month mon)
    {
    using namespace internal;

    const QString MONTHS[] =
        {
        TimeStringConstants::tr ("Jan"),
        TimeStringConstants::tr ("Feb"),
        TimeStringConstants::tr ("Mar"),
        TimeStringConstants::tr ("Apr"),
        TimeStringConstants::tr ("May"),
        TimeStringConstants::tr ("Jun"),
        TimeStringConstants::tr ("Jul"),
        TimeStringConstants::tr ("Aug"),
        TimeStringConstants::tr ("Sep"),
        TimeStringConstants::tr ("Oct"),
        TimeStringConstants::tr ("Nov"),
        TimeStringConstants::tr ("Dec")
        };
    ASSERT_ARRAY_LENGTH (MONTHS, NUM_MONTHS);

    return MONTHS[mon];
    }

///////////////////////////////////////////////////////////////////////////////
/// Get the full name of a unit
///
/// @param[in]  unit        Unit to get name of
///
/// @return     Name of unit
///
///////////////////////////////////////////////////////////////////////////////
inline QString formatWithUnit (int num, unit unit)
    {
    using namespace internal;

    const QString UNITS[] =
        {
        TimeStringConstants::tr ("%n nanosecond(s)", "", num),
        TimeStringConstants::tr ("%n microsecond(s)", "", num),
        TimeStringConstants::tr ("%n millisecond(s)", "", num),
        TimeStringConstants::tr ("%n second(s)", "", num),
        TimeStringConstants::tr ("%n minute(s)", "", num),
        TimeStringConstants::tr ("%n hour(s)", "", num),
        TimeStringConstants::tr ("%n day(s)", "", num),
        TimeStringConstants::tr ("%n week(s)", "", num)
        };
    ASSERT_ARRAY_LENGTH (UNITS, NUM_UNITS);

    return UNITS[unit];
    }

///////////////////////////////////////////////////////////////////////////////
/// Get the unit label
///
/// @param[in]  unit        Unit to get label for
///
/// @return     Label for unit
///
///////////////////////////////////////////////////////////////////////////////
inline QString unitLabel (unit unit)
    {
    using namespace internal;

    const QString UNITS[] =
        {
        TimeStringConstants::tr ("ns"),
        TimeStringConstants::tr ("μs"),
        TimeStringConstants::tr ("ms"),
        TimeStringConstants::tr ("s"),
        TimeStringConstants::tr ("min"),
        TimeStringConstants::tr ("hrs"),
        TimeStringConstants::tr ("days"),
        TimeStringConstants::tr ("wks")
        };
    ASSERT_ARRAY_LENGTH (UNITS, NUM_UNITS);

    return UNITS[unit];
    }

///////////////////////////////////////////////////////////////////////////////
/// Get the conversion from a given unit to nanoseconds
///
/// @param[in]  unit        Unit to get conversion for
///
/// @return     Unit conversion
///
///////////////////////////////////////////////////////////////////////////////
inline std::chrono::nanoseconds unitConversion (unit unit)
    {
    using namespace std::chrono_literals;

    static const std::chrono::nanoseconds CONVERSIONS[] =
        {
        1ns,
        1us,
        1ms,
        1s,
        1min,
        1h,
        24h,
        7 * 24h
        };
    ASSERT_ARRAY_LENGTH (CONVERSIONS, NUM_UNITS);

    return CONVERSIONS[unit];
    }

///////////////////////////////////////////////////////////////////////////////
/// Format a set of days in a week
///
/// @param[in]  dayset      Set of days
///
/// @return     Formatted and translated string
///
///////////////////////////////////////////////////////////////////////////////
QString formatDaysOfTheWeek (const days& dayset);

///////////////////////////////////////////////////////////////////////////////
/// Convert to the best unit for display
///
/// @param[in]  duration    Duration to convert
/// @param[in]  units       Set of units to choose from
///
/// @return     [count,unit]
///
///////////////////////////////////////////////////////////////////////////////
std::pair<int, unit> convertToBestUnit (const std::chrono::nanoseconds& duration,
                                        const units&                    units);

} // utils::time
