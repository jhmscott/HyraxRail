/**
 * @file        utils/os.hpp
 * @brief       Operating system utilities, not provided by QT
 * @author      Justin Scott
 * @date        2026-01-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once


#include <string>
#include <thread>

#include <QThread>

#ifdef Q_OS_WIN
#include <windef.h>
#include <WinUser.h>
#endif // Q_OS_WIN


namespace utils::os
{

// Vibration effect type
enum vibrationEffect
    {
    VIBRATE_TICK = 0,
    VIBRATE_CLICK,
    VIBRATE_LONG_CLICK,
    VIBRATE_DOUBLE_CLICK
    };

inline constexpr auto null_handle = static_cast<std::thread::native_handle_type> (0);

///////////////////////////////////////////////////////////////////////////////
/// Set the name of a thread by the native OS handle
///
/// @param[in]  name        Thread name
/// @param[in]  handle      Native OS thread handle. If NULL, this names the current thread
///
///////////////////////////////////////////////////////////////////////////////
void setThreadName (std::string_view name, std::thread::native_handle_type handle = null_handle);

///////////////////////////////////////////////////////////////////////////////
/// Set the name of a  std::thread
///
/// @param[in]  name        Thread name
/// @param[in]  thread    Thread to name
///
///////////////////////////////////////////////////////////////////////////////
inline void setThreadName (std::string_view name, std::thread& thread)
    {
    setThreadName (name, thread.native_handle ());
    }

#ifdef __cpp_lib_jthread
///////////////////////////////////////////////////////////////////////////////
/// Set the name of a  std::jthread
///
/// @param[in]  name        Thread name
/// @param[in]  thread    Thread to name
///
///////////////////////////////////////////////////////////////////////////////
inline void setThreadName (std::string_view name, std::jthread& thread)
    {
    setThreadName (name, thread.native_handle ());
    }
#endif // __cpp_lib_jthread


///////////////////////////////////////////////////////////////////////////////
/// Get a thread's name
///
/// @param[in]  handle      Native OS thread handle. If NULL, this names the current thread
///
/// @return     Name of the thread
///
///////////////////////////////////////////////////////////////////////////////
std::string getThreadName (std::thread::native_handle_type handle = null_handle);

inline std::string getThreadName (std::thread& thread)
    { return getThreadName (thread.native_handle ()); }

///////////////////////////////////////////////////////////////////////////////
/// Send a push notification to the user
///
/// @param[in]  title                   Notification title
/// @param[in]  description      Notification text
///
///////////////////////////////////////////////////////////////////////////////
void notify (std::string_view title, std::string_view description);

///////////////////////////////////////////////////////////////////////////////
/// Provide haptic feedback to the user
///
/// @param[in]  vibrate     Vibration effect to use
///
///////////////////////////////////////////////////////////////////////////////
void hapticFeedback (vibrationEffect vibrate);

///////////////////////////////////////////////////////////////////////////////
/// Check if IPv6 is available on this machine
///
/// @return     True if IPv6 is available
///
///////////////////////////////////////////////////////////////////////////////
bool isIPv6Available ();

#if defined (Q_OS_WIN) || defined (DOXYGEN)

namespace win32
{

///////////////////////////////////////////////////////////////////////////////
/// Win32 window meta class. Provides registration of the window class
///
/// @remarks    Use DEFINE_WIN32_WINDOW() to associate a meta class instance
///             with a class that manages a win32 window
///
/// @ingroup    META_CLASS
///
///////////////////////////////////////////////////////////////////////////////
class WindowClass
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  classname   Window class name
    /// @param[in]  wndProc     Window proc function pointer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    WindowClass (const wchar_t* classname, WNDPROC wndProc);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the registered class name
    ///
    /// @return     Class name
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const wchar_t* name () const { return m_classname.c_str (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the ATOM for this registered class
    ///
    /// @return     Class ATOM
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ATOM atom () const { return m_atom; }
private:
    const std::wstring  m_classname;    ///< Class name
    ATOM                m_atom;         ///< Class atom
    };


///////////////////////////////////////////////////////////////////////////////
/// Create a win32 window meta class instance for a given class
///
/// @param[in]  type        Class type
///
/// @remarks    type must have a static function wndProc that meets the WNDPROC
///             function signature. This can be achieved by inheriting from
///             NativeWindow
///
/// @ingroup    META_CLASS_MACRO
///
///////////////////////////////////////////////////////////////////////////////
#define DEFINE_WIN32_WINDOW(type) \
public:\
    static const WindowClass& getWindowClassStatic () { return meta; } \
    virtual const WindowClass& getWindowClass () const { return meta; } \
private:\
    static inline const WindowClass meta{ L ## #type, &type::wndProc };


///////////////////////////////////////////////////////////////////////////////
/// Class to manage a native Win32 window
///
///////////////////////////////////////////////////////////////////////////////
class NativeWindow
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  hwnd        Window handle to manage
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit NativeWindow (HWND hwnd);

    // Non-copyable
    NativeWindow (const NativeWindow&) = delete;
    NativeWindow& operator= (const NativeWindow&) = delete;

    ///////////////////////////////////////////////////////////////////////////////
    /// Move constructor
    ///
    /// @param[in,out]  other       Window being moved
    ///
    ///////////////////////////////////////////////////////////////////////////////
    NativeWindow (NativeWindow&& other) noexcept;

    ///////////////////////////////////////////////////////////////////////////////
    /// Move assignment operator
    ///
    /// @param[in,out]  other       Window being moved
    ///
    /// @return         Reference to this
    ///
    ///////////////////////////////////////////////////////////////////////////////
    NativeWindow& operator= (NativeWindow&& other) noexcept;

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~NativeWindow () { close (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get a handle to this message only window
    ///
    /// @return     Window handle
    ///
    ///////////////////////////////////////////////////////////////////////////////
    HWND getHandle () { return m_hwnd; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Close the window
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void close ();

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// Message only window procedure
    ///
    /// @param[in]  hwnd        Window handle
    /// @param[in]  msg         Message type (WM_*)
    /// @param[in]  wParam      Wide param
    /// @param[in]  lParam      Long param
    ///
    /// @return     Message result
    ///
    /// @remarks    Protected so that DEFINE_WIN32_WINDOW() works
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static LRESULT wndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hwnd;    ///< Window handle

    ///////////////////////////////////////////////////////////////////////////////
    /// Implement this to handle the messages you've registered to receieve
    ///
    /// @param[in]  hwnd        Window handle
    /// @param[in]  msg         Message type (WM_*)
    /// @param[in]  wParam      Wide param
    /// @param[in]  lParam      Long param
    ///
    /// @return     Message result
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual LRESULT message (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

    };

///////////////////////////////////////////////////////////////////////////////
/// Message only window class. This is an invisible window that can be used to
/// receive win32 messages
///
///////////////////////////////////////////////////////////////////////////////
class MessageOnlyWindow : public NativeWindow
    {
    DEFINE_WIN32_WINDOW (MessageOnlyWindow);
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    MessageOnlyWindow ();

    };

} // namespace win32

#endif // defined (Q_OS_WIN) || defined (DOXYGEN)


} // namespace utils::os
