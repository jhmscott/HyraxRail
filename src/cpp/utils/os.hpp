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

#include <qwindowdefs.h>

namespace utils::os
{

///////////////////////////////////////////////////////////////////////////////
/// Set the name of a thread by the native OS handle
///
/// @param[in]  name            Thread name
/// @param[in]  handle        Native OS thread handle. If NULL, this names the current thread
///
///////////////////////////////////////////////////////////////////////////////
void setThreadName (std::string_view name, std::thread::native_handle_type handle = NULL);

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
/// Send a push notification to the user
///
/// @param[in]  title                   Notification title
/// @param[in]  description      Notification text
/// 
///////////////////////////////////////////////////////////////////////////////
void notify (std::string_view title, std::string_view description);

} // namespace utils::os
