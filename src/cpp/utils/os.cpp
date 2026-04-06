/**
 * @file        utils/os.cpp
 * @brief       Operating system utilities, not provided by QT
 * @author      Justin Scott
 * @date        2026-04-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <qglobal.h>

#include <res/version.h>
#include <utils/os.hpp>

#ifdef Q_OS_WIN
#include <debugapi.h>
#include <processthreadsapi.h>
#include <libloaderapi.h>
#include <stringapiset.h>
#include <Shobjidl.h>
#endif // Q_OS_WIN

#ifdef Q_OS_UNIX
#include <pthread.h>
#endif // Q_OS_UNIX


namespace utils::os
{

template<size_t N>
static bool utf8ToWString (std::string_view string, wchar_t (&wString)[N])
    {
    int bytes = MultiByteToWideChar (CP_UTF8,
                                     0,
                                     string.data (),
                                     static_cast<int> (string.size ()),
                                     wString,
                                     N - 1);
    if (bytes > 0)
        {
        wString[bytes] = '\0';
        }

    return bytes > 0;
    }



void setThreadName (std::string_view name, std::thread::native_handle_type handle)
    {
    if (NULL == handle)
        {
#ifdef Q_OS_WIN
        handle = GetCurrentThread ();
#endif // Q_OS_WIN

#ifdef Q_OS_UNIX
        handle = pthread_self ();
#endif // Q_OS_UNIX
        }

#ifdef Q_OS_WIN
    wchar_t wThreadName[255];

    if (not utf8ToWString (name, wThreadName))
        {
        // ERROR Handling
        }
    else
        {
        SetThreadDescription (handle, wThreadName);
        }

#ifdef _DEBUG
    // Older versions of visual studio don't support SetThreadDesciption()
    // Check what version we are using
    //
    if (TRUE == IsDebuggerPresent ())
        {

        }
#endif // _DEBUG

#endif // Q_OS_WIN

#ifdef Q_OS_UNIX
    char    truncated[16];
    size_t  numToCopy = std::min (name.size (),
                                  std::size (truncated) - 1);
    strncpy (truncated,
             name.data (),
             numToCopy);
    truncated[numToCopy] = '\0';

    pthread_setname_np (hanle, name);
#endif // Q_OS_UNIX

    }

void notify (std::string_view title, std::string_view description)
    {
#ifdef Q_OS_WIN
    WCHAR               wTitle[256];
    WCHAR               wDescription[2048];

    if (utf8ToWString (title, wTitle) &&
        utf8ToWString (description, wDescription))
        {
        std::wstring wTitle2        = wTitle;
        std::wstring wDescription2  = wDescription;

        std::thread th (
            [wTitle2        = std::move (wTitle2),
             wDescription2  = std::move (wDescription2)] ()
            {
            IUserNotification*  notification = NULL;
            HRESULT             hr;

            hr = CoCreateInstance (CLSID_UserNotification,
                                   NULL,
                                   CLSCTX_LOCAL_SERVER,
                                   IID_IUserNotification,
                                   reinterpret_cast<void**> (&notification));

            if (FAILED (hr))
                {
                // Log error
                }
            else
                {
                HICON hIcon = LoadIcon (NULL, IDI_QUESTION);

                notification->SetBalloonInfo (wTitle2.c_str (),
                                              wDescription2.c_str (),
                                              NIIF_INFO);
                notification->SetIconInfo (hIcon, TEXT (PRODUCT_NAME_FULL));
                notification->SetBalloonRetry (5000, 0, 0);
                notification->Show (NULL, 5000);
                notification->Release ();
                }
            });

        setThreadName ("Notification", th);

        th.detach ();
        }

#endif // Q_OS_WIN
    }

} // namespace utils::os