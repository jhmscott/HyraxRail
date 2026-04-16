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


#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include <string>


#ifdef Q_OS_WIN

// Internal macro logic, do not use directly
#define __logWinInternal(level, func, ...) \
    (level() << #func "(" << utils::log::internal::passOptionalParameter (__VA_ARGS__) << \
    ") failed: " << utils::log::internal::getLastErrorAsString () << "(" << GetLastError () << ")")

// Log the last windows error code (GetLastError())

#define logWinDebug(func, ...)      (__logWinInternal (qDebug,      func, __VA_ARGS__))
#define logWinInfo(func, ...)       (__logWinInternal (qInfo,       func, __VA_ARGS__))
#define logWinWarning(func, ...)    (__logWinInternal (qWarning,    func, __VA_ARGS__))
#define logWinCrtical(func, ...)    (__logWinInternal (qCritical,   func, __VA_ARGS__))
#define logWinFatal(func, ...)      (__logWinInternal (qFatal,      func, __VA_ARGS__))

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
inline std::string getLastErrorAsString ()
    {        //Get the error message ID, if any.
    DWORD errorMessageID = GetLastError ();
    LPSTR messageBuffer = nullptr;
    std::string message;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                  FORMAT_MESSAGE_FROM_SYSTEM |
                                  FORMAT_MESSAGE_IGNORE_INSERTS,
                                  NULL,
                                  errorMessageID,
                                  MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                                  reinterpret_cast<LPSTR> (&messageBuffer),
                                  0,
                                  NULL);

    if (NULL != messageBuffer)
        {
        // Remove the trailing \r\n
        messageBuffer[size - 1] = '\0';

         //Copy the error message into a std::string.
        message = { messageBuffer, size - 2 };


        //Free the Win32's string's buffer.
        LocalFree (messageBuffer);
        }

    return message;
    }
#endif // Q_OS_WIN


///////////////////////////////////////////////////////////////////////////////
/// Returns an empty string. Used to format the error message if no parameter is passed
///
/// @return     Empty string
///
///////////////////////////////////////////////////////////////////////////////
inline const char* passOptionalParameter ()
    {
    return "";
    }

///////////////////////////////////////////////////////////////////////////////
///  Pass a value for formatting
///
/// @tparam     T             Value type
///
/// @param[in]  val         Value to pass
///
/// @return     val, unchanged
///////////////////////////////////////////////////////////////////////////////
template<class T>
T passOptionalParameter (T val)
    {
    return val;
    }
} // namespace internal

} // namespace utils::log
