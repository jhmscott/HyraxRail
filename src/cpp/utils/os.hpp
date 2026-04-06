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

void setThreadName (std::string_view name, std::thread::native_handle_type handle = NULL);

inline void setThreadName (std::string_view name, std::thread& thread)
    {
    setThreadName (name, thread.native_handle ());
    }

#ifdef __cpp_lib_jthread
inline void setThreadName (std::string_view name, std::jthread& thread)
    {
    setThreadName (name, thread.native_handle ());
    }
#endif // __cpp_lib_jthread


void notify (std::string_view title, std::string_view description);

}