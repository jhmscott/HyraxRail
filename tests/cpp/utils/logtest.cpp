/**
 * @file        utils/logtest.hpp
 * @brief       Test suite for the log helper functions
 * @author      Justin Scott
 * @date        2026-08-15
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <testutils/logspy.hpp>

#include <utils/log.hpp>

#include <QtTest>

#ifdef Q_OS_WIN
#include <winerror.h>
#endif // Q_OS_WIN

#define LOGTEST_LINE_START  314159


///////////////////////////////////////////////////////////////////////////////
/// Assert the fields of a log record are what we expect
///
/// @param[in]  record      Record to check
/// @param[in]  function    Expected function in log message
/// @param[in]  code        Expected error code
/// @param[in]  message     Expected error message
/// @param[in]  type        Log level
/// @param[in]  lineOffset  Number of lines sine #line LOGTEST_LINE_START
/// @param[in]  param       Expected function parameter
///
///////////////////////////////////////////////////////////////////////////////
static void assertLogRecord (const testutils::logRecord&    record,
                             const QString&                 function,
                             int                            code,
                             const QString&                 message,
                             QtMsgType                      type,
                             int                            lineOffset,
                             const char*                    param = "")
    {
    QCOMPARE (record.type, type);

    QCOMPARE (record.file, __FILE__);
    QCOMPARE (record.line, LOGTEST_LINE_START + lineOffset);

    QString expected = function + "(" + param + ") failed: " +
                       message + "(" + QString::number (code) + ")";

    QCOMPARE (record.msg, expected);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the log helper functions
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class LogTest : public QObject
    {
    Q_OBJECT
private slots:

#if defined (Q_OS_WIN) || defined (DOXYGEN)

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for logWinTest() test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void logWinTest_data ()
        {
        QTest::addColumn<int> ("code");
        QTest::addColumn<QString> ("message");

        // see https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
        QTest::newRow ("0") << 0 << "The operation completed successfully.";
        QTest::newRow ("1") << 1 << "Incorrect function.";
        QTest::newRow ("2") << 2 << "The system cannot find the file specified.";
        QTest::newRow ("6") << 2 << "The handle is invalid.";

        // see https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--500-999-
        QTest::newRow ("534") << 534 << "Arithmetic result exceeded 32 bits.";
        QTest::newRow ("536") << 536 << "Waiting for a process to open the other end of the pipe.";
        QTest::newRow ("546") << 546 << "Length of message passed to NtRequestPort or "
                                        "NtRequestWaitReplyPort was longer than the maximum message allowed by the port.";

        // see https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--1000-1299-
        QTest::newRow ("1015") << 1015 << "The registry is corrupted. "
                                          "The structure of one of the files containing registry data is corrupted, "
                                          "or the system's memory image of the file is corrupted, "
                                          "or the file could not be recovered because the alternate copy "
                                          "or log was absent or corrupted.";
        QTest::newRow ("1064") << 1064 << "An exception occurred in the service when handling the control request.";

        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the logWin*() functions
    ///
    /// @see    logWinDebug()
    /// @see    logWinInfo()
    /// @see    logWinWarning()
    /// @see    logWinCritical()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void logWinTest ()
        {
        QFETCH (int, code);
        QFETCH (QString, message);

        testutils::LogSpy spy;

        SetLastError (code);

        // Reset line number, so the logged lines are consistent
#line LOGTEST_LINE_START
        logWinDebug     (HandleToULong);
        SetLastError    (code);
        logWinInfo      (HandleToULong);
        SetLastError    (code);
        logWinWarning   (HandleToULong);
        SetLastError    (code);
        logWinCritical  (HandleToULong);

        SetLastError    (code);
        logWinDebug     (HandleToULong, "1");
        SetLastError    (code);
        logWinInfo      (HandleToULong, "2");
        SetLastError    (code);
        logWinWarning   (HandleToULong, "3");
        SetLastError    (code);
        logWinCritical  (HandleToULong, "4");

        std::lock_guard lk{ spy };

        QCOMPARE (spy.size (), 8);

        assertLogRecord (spy[0],
                         "HandleToULong",
                         code,
                         message,
                         QtDebugMsg,
                         0);
        assertLogRecord (spy[1],
                         "HandleToULong",
                         code,
                         message,
                         QtInfoMsg,
                         2);
        assertLogRecord (spy[2],
                         "HandleToULong",
                         code,
                         message,
                         QtWarningMsg,
                         4);
        assertLogRecord (spy[3],
                         "HandleToULong",
                         code,
                         message,
                         QtCriticalMsg,
                         6);


        assertLogRecord (spy[4],
                         "HandleToULong",
                         code,
                         message,
                         QtDebugMsg,
                         9,
                         "1");
        assertLogRecord (spy[5],
                         "HandleToULong",
                         code,
                         message,
                         QtInfoMsg,
                         11,
                         "2");
        assertLogRecord (spy[6],
                         "HandleToULong",
                         code,
                         message,
                         QtWarningMsg,
                         13,
                         "3");
        assertLogRecord (spy[7],
                         "HandleToULong",
                         code,
                         message,
                         QtCriticalMsg,
                         15,
                         "4");
        }
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for logHrTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void logHrTest_data ()
        {
        QTest::addColumn<int> ("code");
        QTest::addColumn<QString> ("message");

        // see https://learn.microsoft.com/en-us/windows/win32/seccrypto/common-hresult-values
        QTest::newRow ("OK")        << (int) S_OK         << "The operation completed successfully.";
        QTest::newRow ("NOTIMPL")   << (int) E_NOTIMPL    << "Not implemented";
        QTest::newRow ("OINTERFACE")<< (int) E_NOINTERFACE<< "No such interface supported";
        QTest::newRow ("POINTER")   << (int) E_POINTER    << "Pointer that is not valid";
        QTest::newRow ("ABORT")     << (int) E_ABORT      << "Operation aborted";
        QTest::newRow ("FAIL")      << (int) E_FAIL       << "Unspecified failure";
        QTest::newRow ("UNEXPECTED")<< (int) E_UNEXPECTED << "Unexpected failure";
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the logHr*() functions
    ///
    /// @see    logHrDebug()
    /// @see    logHrInfo()
    /// @see    logHrWarning()
    /// @see    logHrCritical()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void logHrTest ()
        {
        QFETCH (int, code);
        QFETCH (QString, message);

        testutils::LogSpy spy;

        // Reset line number, so the logged lines are consistent
#line LOGTEST_LINE_START
        logHrDebug      (SetThreadDescription, code);
        logHrInfo       (SetThreadDescription, code);
        logHrWarning    (SetThreadDescription, code);
        logHrCritical   (SetThreadDescription, code);

        logHrDebug      (SetThreadDescription, code, "1");
        logHrInfo       (SetThreadDescription, code, "2");
        logHrWarning    (SetThreadDescription, code, "3");
        logHrCritical   (SetThreadDescription, code, "4");

        std::lock_guard lk{ spy };

        QCOMPARE (spy.size (), 8);

        assertLogRecord (spy[0],
                         "SetThreadDescription",
                         code,
                         message,
                         QtDebugMsg,
                         0);
        assertLogRecord (spy[1],
                         "SetThreadDescription",
                         code,
                         message,
                         QtInfoMsg,
                         1);
        assertLogRecord (spy[2],
                         "SetThreadDescription",
                         code,
                         message,
                         QtWarningMsg,
                         2);
        assertLogRecord (spy[3],
                         "SetThreadDescription",
                         code,
                         message,
                         QtCriticalMsg,
                         3);


        assertLogRecord (spy[4],
                         "SetThreadDescription",
                         code,
                         message,
                         QtDebugMsg,
                         5,
                         "1");
        assertLogRecord (spy[5],
                         "SetThreadDescription",
                         code,
                         message,
                         QtInfoMsg,
                         6,
                         "2");
        assertLogRecord (spy[6],
                         "SetThreadDescription",
                         code,
                         message,
                         QtWarningMsg,
                         7,
                         "3");
        assertLogRecord (spy[7],
                         "SetThreadDescription",
                         code,
                         message,
                         QtCriticalMsg,
                         8,
                         "4");
        }
#endif // Q_OS_WIN


    };

QTEST_GUILESS_MAIN (LogTest)

#include "logtest.moc"
