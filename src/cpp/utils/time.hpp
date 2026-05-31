/**
 * @file        utils/time.hpp
 * @brief       Time utilities
 * @author      Justin Scott
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QDateTime>

#include <chrono>

namespace utils::time
{
// System clock time point type
template<class Duration>
using system_time = std::chrono::time_point<std::chrono::system_clock, Duration>;


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

} // utils::time
