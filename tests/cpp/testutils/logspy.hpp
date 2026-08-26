/**
 * @file        testutils/logspy.hpp
 * @brief       Class to capture log messages for unit testing
 * @author      Justin Scott
 * @date        2026-08-15
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <common.hpp>

#include <QtLogging>

#include <mutex>

namespace testutils
{

// Log message record
struct logRecord
    {
    QtMsgType   type;   ///< Log level
    QString     file;   ///< Source file
    int         line;   ///< Source line
    QString     msg;    ///< Log message
    };

///////////////////////////////////////////////////////////////////////////////
/// Class to capture log message for unit testing
///
///////////////////////////////////////////////////////////////////////////////
class LogSpy : public std::vector<logRecord>
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    LogSpy ()
        {
        if (NULL != instance)
            {
            throw std::runtime_error{ "Only one spy instance at once" };
            }

        instance = this;

        m_oldHandler = qInstallMessageHandler (&LogSpy::messageHandler);
        }

    // non-copyable
    LogSpy (const LogSpy&) = delete;
    LogSpy& operator= (const LogSpy&) = delete;

    // non-movable
    LogSpy (LogSpy&&) = delete;
    LogSpy& operator= (LogSpy&&) = delete;

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~LogSpy ()
        {
        std::lock_guard lk{ *this };
        qInstallMessageHandler (m_oldHandler);
        instance = NULL;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Lock the spy instance to peak into the captured message
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void lock () { m_mtx.lock (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Unlock the spy instance
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void unlock () noexcept { m_mtx.unlock (); }

private:
    QtMessageHandler    m_oldHandler;       ///< Previous handler
    std::mutex          m_mtx;              ///< Protects the internal list of messages

    static inline LogSpy* instance = NULL;  ///< Single LogSpy instance

    ///////////////////////////////////////////////////////////////////////////////
    /// Qt log message handler callback to collect the log messaged
    ///
    /// @param[in]  type        Log message level
    /// @param[in]  ctx         Log context (source location)
    /// @param[in]  msg         Log message text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static void messageHandler (QtMsgType                   type,
                                const QMessageLogContext&   ctx,
                                const QString&              msg)
        {
        std::lock_guard lk{ *instance };

        instance->emplace_back (logRecord{ type, ctx.file, ctx.line, msg });

        if (NULL != instance->m_oldHandler)
            {
            instance->m_oldHandler (type, ctx, msg);
            }
        }

    };

} // namespace testutils