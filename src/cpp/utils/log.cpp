/**
 * @file        utils/log.cpp
 * @brief       QT logging helpers
 * @author      Justin Scott
 * @date        2026-08-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/log.hpp>


#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

#include <comdef.h>
#endif


namespace utils::log::internal
{
#ifdef Q_OS_WIN
QString getLastErrorAsString ()
    {        //Get the error message ID, if any.
    DWORD   errorMessageID = GetLastError ();
    LPWSTR  messageBuffer = NULL;
    QString message;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageW (FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                  FORMAT_MESSAGE_FROM_SYSTEM |
                                  FORMAT_MESSAGE_IGNORE_INSERTS,
                                  NULL,
                                  errorMessageID,
                                  MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                                  reinterpret_cast<LPWSTR> (&messageBuffer),
                                  0,
                                  NULL);

    if (NULL != messageBuffer)
        {
        // Remove the trailing \r\n
        messageBuffer[size - 1] = '\0';

        message = QString::fromWCharArray (messageBuffer, size - 2);

        LocalFree (messageBuffer);
        }

    return message;
    }

QString hresultToString (HRESULT hr)
    {
    _com_error err{ hr };

    return QString::fromWCharArray (err.ErrorMessage ());
    }

#endif // Q_OS_WIN

QDebug prepStream (QDebug stream)
    {
    stream.noquote ();
    stream.nospace ();

    return stream;
    }

} // namespace utils::log::internal