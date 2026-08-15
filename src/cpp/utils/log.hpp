/**
 * @file        utils/log.hpp
 * @brief       QT logging helpers
 * @author      Justin Scott
 * @date        2026-01-31
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <QDebug>
#include <qlogging.h>

#include <string>

#ifdef Q_OS_WIN

#include <errhandlingapi.h>

// Internal macro logic, do not use directly
#define __logWinInternal(level, func, ...) \
    (utils::log::internal::prepStream (level()) << #func "(" << utils::log::internal::passOptionalParameter (__VA_ARGS__) << \
    ") failed: " << utils::log::internal::getLastErrorAsString () << "(" << GetLastError () << ")")

// Log the last windows error code (GetLastError())

#define logWinDebug(func, ...)      (__logWinInternal (qDebug,      func, __VA_ARGS__))
#define logWinInfo(func, ...)       (__logWinInternal (qInfo,       func, __VA_ARGS__))
#define logWinWarning(func, ...)    (__logWinInternal (qWarning,    func, __VA_ARGS__))
#define logWinCritical(func, ...)   (__logWinInternal (qCritical,   func, __VA_ARGS__))
#define logWinFatal(func, ...)      (__logWinInternal (qFatal,      func, __VA_ARGS__))

#define __logHrInternal(level, func, hr, ...) \
    (utils::log::internal::prepStream (level()) << #func "(" << utils::log::internal::passOptionalParameter (__VA_ARGS__) << \
    ") failed: " << utils::log::internal::hresultToString (hr) << "(" << hr << ")")


#define logHrDebug(func, hr, ...)      (__logHrInternal (qDebug,      func, hr, __VA_ARGS__))
#define logHrInfo(func, hr, ...)       (__logHrInternal (qInfo,       func, hr, __VA_ARGS__))
#define logHrWarning(func, hr, ...)    (__logHrInternal (qWarning,    func, hr, __VA_ARGS__))
#define logHrCritical(func, hr, ...)   (__logHrInternal (qCritical,   func, hr, __VA_ARGS__))
#define logHrFatal(func, hr, ...)      (__logHrInternal (qFatal,      func, hr, __VA_ARGS__))

#endif // Q_OS_WIN

namespace utils::log
{

namespace internal
{

#ifdef Q_OS_WIN

///////////////////////////////////////////////////////////////////////////////
/// Get the string error message for the GetLastError() value
///
/// @return     String error message for GetLastError ()
///
///////////////////////////////////////////////////////////////////////////////
QString getLastErrorAsString ();

///////////////////////////////////////////////////////////////////////////////
/// Get the string error message for an HRESULT
///
/// @param[in]  hr      HRESULT error code
///
/// @return     String error message
///
///////////////////////////////////////////////////////////////////////////////
QString hresultToString (HRESULT hr);

#endif // Q_OS_WIN

///////////////////////////////////////////////////////////////////////////////
/// Apply settings to a debug stream for formatting logWin*() and logHr*()
///
/// @param[in]  stream      Un-prepared stream
///
/// @return     Stream with settings
///
///////////////////////////////////////////////////////////////////////////////
QDebug prepStream (QDebug stream);

///////////////////////////////////////////////////////////////////////////////
/// Returns an empty string. Used to format the error message if no parameter is passed
///
/// @return     Empty string
///
///////////////////////////////////////////////////////////////////////////////
inline const char* passOptionalParameter () { return ""; }

///////////////////////////////////////////////////////////////////////////////
///  Pass a value for formatting
///
/// @tparam     T       Value type
///
/// @param[in]  val     Value to pass
///
/// @return     val, unchanged
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
T passOptionalParameter (T val) { return val; }
} // namespace internal

} // namespace utils::log
