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
#include <utils/log.hpp>
#include <utils/os.hpp>

#include <QNetworkInterface>
#include <QHostAddress>

#ifdef Q_OS_WIN
#include <debugapi.h>
#include <processthreadsapi.h>
#include <libloaderapi.h>
#include <stringapiset.h>
#include <Shobjidl.h>
#include <shellapi.h>
#endif // Q_OS_WIN

#ifdef Q_OS_UNIX
#include <pthread.h>
#endif // Q_OS_UNIX


namespace utils::os
{

// local helper functions

#ifdef Q_OS_WIN
///////////////////////////////////////////////////////////////////////////////
/// Convert a UTF-8 encoded char string to a UTF-16 encoded wide char string
///
/// @tparam     N                       Size of output string, including NUL
///
/// @param[in]  string            UTF-8 string
/// @param[out] sWstring        UTF-16 string
///
/// @return     true if the string could be converted
///////////////////////////////////////////////////////////////////////////////
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
#endif // Q_OS_WIN

// Exported functions

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

#endif // Q_OS_WIN

#ifdef Q_OS_UNIX
    char    truncated[16];
    size_t  numToCopy = std::min (name.size (),
                                  std::size (truncated) - 1);
    strncpy (truncated,
             name.data (),
             numToCopy);
    truncated[numToCopy] = '\0';


#ifdef Q_OS_MAC
    if (pthread_self () == handle)
        {
        pthread_setname_np (truncated);
        }
#else
    pthread_setname_np (handle, truncated);
#endif // Q_OS_MAC
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

bool isIPv6Available ()
    {
    bool hasIpv6 = false;

    // Loop through available network adapters
    for (const QNetworkInterface& netInterface : QNetworkInterface::allInterfaces ())
        {
        // Filter out the loopback and adapters in the "down" link state
        if (    netInterface.flags ().testFlag (QNetworkInterface::IsUp) &&
            not netInterface.flags ().testFlag (QNetworkInterface::IsLoopBack))
            {
            // Loop through each address for that netowrk adapter
            for (const QNetworkAddressEntry& entry : netInterface.addressEntries ())
                {
                // If it's an IPv6 address and it's not in the link local address state,
                // which typically indicates no DHCP lease has been acquired
                if (    entry.ip ().protocol () == QAbstractSocket::IPv6Protocol &&
                    not entry.ip ().isLinkLocal ())
                    {
                    hasIpv6 = true;
                    break;
                    }
                }
            }

        if (hasIpv6)
            {
            // Stop search early
            break;
            }
        }

    return hasIpv6;
    }


#ifdef Q_OS_WIN

namespace win32
{


WindowClass::WindowClass (const wchar_t* classname, LRESULT (*wndProc)(HWND, UINT, WPARAM, LPARAM)) :
    m_classname (classname)
    {
    WNDCLASSEX wndclass{ 0 };

    wndclass.cbSize = sizeof (WNDCLASSEX);
    wndclass.lpszClassName = classname;
    wndclass.lpfnWndProc = wndProc;
    wndclass.hInstance = GetModuleHandle (NULL);

    if (NULL == (m_atom = RegisterClassEx (&wndclass)))
        {
        logWinWarning (RegisterClassEx, classname);
        }
    }


NativeWindow::NativeWindow (HWND hwnd) :
    m_hwnd (hwnd)
    {
    if (NULL == m_hwnd)
        {
        logWinWarning (CreateWindowEx);
        throw std::runtime_error ("CreateWindowEx() failed");
        }
    else
        {
        SetWindowLongPtr (m_hwnd,
                          GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR> (this));
        }
    }

NativeWindow::NativeWindow (NativeWindow&& other) noexcept :
    m_hwnd (NULL)
    { std::swap (m_hwnd, other.m_hwnd); }


NativeWindow& NativeWindow::operator=(NativeWindow&& other) noexcept
    {
    if (&other != this)
        {
        close ();
        std::swap (other.m_hwnd, m_hwnd);
        }

    return *this;
    }

LRESULT NativeWindow::wndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
    LRESULT         res = FALSE;
    NativeWindow*   wnd = reinterpret_cast<NativeWindow*>
                                (GetWindowLongPtr (hwnd, GWLP_USERDATA));

    if (NULL != wnd)
        {
        res = wnd->message (hwnd, msg, wParam, lParam);
        }
    else
        {
        res = DefWindowProc (hwnd, msg, wParam, lParam);
        }

    return res;
    }

void NativeWindow::close ()
    {
    if (NULL != m_hwnd)
        {
        SetWindowLongPtr (m_hwnd,
                          GWLP_USERDATA,
                          NULL);

        DestroyWindow (m_hwnd);
        m_hwnd = NULL;
        }
    }


MessageOnlyWindow::MessageOnlyWindow () :
    NativeWindow (CreateWindowEx (0,     // EX Style
                                  reinterpret_cast<WCHAR*> (getWindowClass ().atom ()),
                                  L"Message only window",
                                  0,     // Style
                                  0,     // X
                                  0,     // Y
                                  0,     // Width
                                  0,     // Height
                                  HWND_MESSAGE,
                                  NULL,  // hMenu
                                  GetModuleHandle (NULL),
                                  NULL)) // lpParam
    {}


} // namespace win32

#endif // Q_OS_WIN

} // namespace utils::os
